<?php

/**
 * @file
 * Administrative functionality for managing key configurations.
 */

/**
 * Menu callback; displays the list of key configurations.
 */
function key_ui_key_configs_list() {
  $configs = key_get_configs();

  $header = array(
    t('Name'),
    t('Provider'),
    t('Type'),
    t('Status'),
    array('data' => t('Operations'), 'colspan' => '2'),
  );
  $rows = array();

  foreach ($configs as $name => $config) {
    $label = $config['label'];
    $name = $config['name'];
    $description = $config['description'];
    $provider = key_get_provider($config['provider']);
    $type = key_get_type($config['type']);

    // Get the function to get the status of the key.
    $status_function = ctools_plugin_get_function($provider, 'status callback');

    // If there are any settings, use them.
    $provider_settings = isset($config['provider_settings']) ? $config['provider_settings'] : array();

    // Get the status of the key.
    $status = call_user_func($status_function, $provider_settings);

    $config_url_string = str_replace('_', '-', $name);

    $variables = array(
      'label' => $label,
      'name' => $name,
      'description' => $description,
    );

    // Set the name column.
    $row = array(theme('key_ui_key_configs_list_description', $variables));

    // Set the key provider column.
    $row[] = array('data' => $provider['title']);

    // Set the type column.
    $row[] = array('data' => $type['title']);

    // Set the status column.
    $row[] = array('data' => $status['message']);

    // Set the edit column.
    $row[] = array('data' => l(t('edit'), KEY_MENU_PATH . '/edit/' . $config_url_string));

    // Set the delete column.
    $row[] = array('data' => l(t('delete'), KEY_MENU_PATH . '/delete/' . $config_url_string));

    $rows[] = $row;
  }

  $build['key_ui_configs_list_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No keys are available. <a href="@link">Add a key</a>.', array('@link' => url(KEY_MENU_PATH . '/add'))),
  );

  return $build;
}

/**
 * Form constructor for the key configuration edit form.
 *
 * @param array $config
 *   (optional) An array representing the configuration, when editing an
 *   existing configuration.
 *
 * @ingroup forms
 */
function key_ui_key_config_form($form, &$form_state, $config = NULL) {
  // Clear the plugin cache on the first page load, but not on AJAX refreshes.
  if (!isset($form_state['values'])) {
    _key_clear_plugin_cache();
  }

  // Get all valid key providers.
  $providers = key_get_providers(FALSE);

  // Get all valid key providers as options.
  $provider_options = key_get_providers_as_options(FALSE);

  // Determine the key provider.
  if (isset($form_state['values']['provider'])) {
    $provider = $form_state['values']['provider'];
  }
  elseif (isset($config['provider'])) {
    $provider = $config['provider'];
  }
  else {
    $provider = NULL;
  }

  // Get all valid types as options.
  $type_options = key_get_types_as_options(FALSE);

  $form['label'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => $config['label'],
    '#description' => t('The human-readable name of the key.'),
    '#required' => TRUE,
    '#size' => 30,
  );
  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => $config['name'],
    '#maxlength' => 32,
    '#disabled' => isset($config['name']),
    '#machine_name' => array(
      'exists' => 'key_ui_config_load',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for the key. It must only contain lowercase letters, numbers, and underscores.'),
  );
  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textarea',
    '#default_value' => $config['description'],
    '#description' => t('A short description of the key.'),
  );
  $form['type'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#description' => t('The type of key.'),
    '#required' => TRUE,
    '#options' => $type_options,
    '#default_value' => $config['type'],
  );
  $form['provider'] = array(
    '#type' => 'select',
    '#title' => t('Key provider'),
    '#description' => t('The key provider to use.'),
    '#required' => TRUE,
    '#options' => $provider_options,
    '#default_value' => $config['provider'],
    '#ajax' => array(
      'method' => 'replace',
      'callback' => 'key_ui_provider_extras_ajax',
      'wrapper' => 'key-provider-extras-wrapper',
    ),
  );
  $form['provider_extras'] = array(
    '#type' => 'container',
    '#prefix' => '<div id="key-provider-extras-wrapper">',
    '#suffix' => '</div>',
  );
  $form['provider_extras']['provider_settings'] = array(
    '#type' => 'container',
    '#title' => t('Key provider settings'),
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );
  if ($provider && $provider_settings_form = ctools_plugin_get_function($providers[$provider], 'settings form')) {
    $form['provider_extras']['provider_settings']['#type'] = 'fieldset';
    $form['provider_extras']['provider_settings'] += call_user_func($provider_settings_form, $config['provider_settings']);
  }

  $form['provider_extras']['provider_key'] = array(
    '#type' => 'container',
  );
  if ($provider && $provider_key_form = ctools_plugin_get_function($providers[$provider], 'key form')) {
    $key = !empty($config['name']) ? key_get_key($config['name']) : '';
    $form['provider_extras']['provider_key'] += call_user_func($provider_key_form, array('key' => $key));
  }

  $form['provider_extras']['provider_instructions'] = array(
    '#type' => 'container',
    '#title' => t('Instructions'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  if ($provider && $provider_instructions = ctools_plugin_get_function($providers[$provider], 'instructions')) {
    $form['provider_extras']['provider_instructions']['#type'] = 'fieldset';
    $form['provider_extras']['provider_instructions'] += call_user_func($provider_instructions);
  }

  $form['created'] = array(
    '#type' => 'value',
    '#value' => $config['created'],
  );

  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save key'),
    '#submit' => array('key_ui_key_config_form_submit'),
    '#weight' => 40,
  );
  if (isset($config['name'])) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete key'),
      '#submit' => array('key_ui_config_form_delete_submit'),
      '#limit_validation_errors' => array(),
      '#weight' => 45,
    );
  }
  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => KEY_MENU_PATH,
    '#weight' => 50,
  );

  return $form;
}

/**
 * Callback for AJAX form re-rendering for provider additional settings.
 */
function key_ui_provider_extras_ajax($form, $form_state) {
  return $form['provider_extras'];
}

/**
 * Form submission handler for key_ui_key_config_form().
 */
function key_ui_key_config_form_submit($form, &$form_state) {
  $provider = key_get_provider($form_state['values']['provider']);

  foreach (array($provider) as $plugin) {
    if ($submit_callback = ctools_plugin_get_function($plugin, 'submit callback')) {
      // Create a copy so that the plugin callback cannot change the
      // form state.
      $form_state_copy = $form_state;
      call_user_func($submit_callback, $form, $form_state_copy);
    }
  }

  $fields = array(
    'name' => (string) $form_state['values']['name'],
    'label' => (string) $form_state['values']['label'],
    'description' => (string) $form_state['values']['description'],
    'type' => (string) $form_state['values']['type'],
    'provider' => (string) $form_state['values']['provider'],
    'provider_settings' => serialize($form_state['values']['provider_settings']),
    'created' => (string) $form_state['values']['created'],
  );

  $key = isset($form_state['values']['key']) ? $form_state['values']['key'] : NULL;

  key_save_config($fields, $key);

  $form_state['redirect'] = KEY_MENU_PATH . '/list';
}

/**
 * Form submission handler for key_ui_key_config_form().
 *
 * Handles the 'Delete' button on the key configuration form.
 */
function key_ui_config_form_delete_submit($form, &$form_state) {
  $form_state['redirect'] = KEY_MENU_PATH . '/delete/' . str_replace('_', '-', $form['name']['#default_value']);
}

/**
 * Menu callback to delete a key configuration.
 */
function key_ui_key_config_delete_confirm($form, &$form_state, $config) {
  $form['name'] = array('#type' => 'value', '#value' => $config['name']);
  $form['label'] = array('#type' => 'value', '#value' => $config['label']);

  $message = t('Are you sure you want to delete the key %label?', array('%label' => $config['label']));
  $caption = '<p>' . t('This action cannot be undone.') . '</p>';

  return confirm_form($form, filter_xss_admin($message), KEY_MENU_PATH, filter_xss_admin($caption), t('Delete'));
}

/**
 * Submit handler for key_ui_config_delete_confirm.
 */
function key_ui_key_config_delete_confirm_submit($form, &$form_state) {
  db_delete('key_config')
    ->condition('name', $form_state['values']['name'])
    ->execute();

  $t_args = array('%label' => $form_state['values']['label']);
  drupal_set_message(t('The key %label has been deleted.', $t_args));
  watchdog('key', 'Deleted key %label.', $t_args, WATCHDOG_NOTICE);

  $form_state['redirect'] = KEY_MENU_PATH;
}

/**
 * Returns HTML for a key configuration description.
 *
 * @param array $variables
 *   An associative array containing:
 *   - label: The human-readable label of the configuration.
 *   - name: The machine name of the configuration.
 *   - description: A brief description of the configuration.
 *
 * @ingroup themeable
 */
function theme_key_ui_key_configs_list_description($variables) {
  $label = $variables['label'];
  $name = $variables['name'];
  $description = $variables['description'];

  $output = check_plain($label);
  $output .= ' <small>' . t('(Machine name: @name)', array('@name' => $name)) . '</small>';
  $output .= '<div class="description">' . filter_xss_admin($description) . '</div>';

  return $output;
}

/**
 * Menu callback; displays the list of key integrations.
 */
function key_ui_key_integration_form($form, &$form_state) {
  // Clear the plugin cache on the first page load, but not on AJAX refreshes.
  if (!isset($form_state['values'])) {
    _key_clear_plugin_cache('key_integration');
  }

  $integrations = key_get_integrations();
  $modules = system_rebuild_module_data();

  $form['#tree'] = TRUE;
  $form['integrations'] = array();
  foreach ($integrations as $name => $integration) {
    $type_label = !empty($integration['type']) ? $integration['type'] : t('Other');

    $form['integrations'][$type_label][$name] = array(
      'enabled' => array(
        '#type' => 'checkbox',
        '#title' => t('Enabled'),
        '#default_value' => $integration['enabled'],
      ),
      'name' => array(
        '#markup' => $integration['title'],
      ),
      'description' => array(
        '#markup' => $integration['description'],
      ),
      'module' => array(
        '#markup' => $modules[$integration['module']]->info['name'],
      ),
    );
    if (isset($integration['locked']) && $integration['locked'] == TRUE) {
      $form['integrations'][$type_label][$name]['enabled']['#disabled'] = TRUE;
    }
  }

  // Create the fieldsets for each integration type.
  foreach (element_children($form['integrations']) as $type) {
    $form['integrations'][$type] += array(
      '#type' => 'fieldset',
      '#title' => $type,
      '#collapsible' => TRUE,
      '#theme' => 'key_ui_key_integrations_fieldset',
      '#header' => array(
        array('data' => t('Enabled'), 'class' => array('checkbox')),
        t('Name'),
        t('Description'),
        t('Module'),
      ),
    );
  }

  // Sort the fieldsets by title.
  uasort($form['integrations'], 'element_sort_by_title');

  // Add the submit button.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save integrations'),
  );

  return $form;
}

/**
 * Form submission handler for key_ui_key_integration_form().
 */
function key_ui_key_integration_form_submit($form, &$form_state) {
  if (!isset($form_state['values']['integrations'])) {
    return;
  }

  $fields = array();
  foreach ($form_state['values']['integrations'] as $type => $integrations) {
    foreach ($integrations as $name => $integration) {
      $fields['name'] = $name;

      if ($integration['enabled']) {
        $fields['enabled'] = 1;
      }
      else {
        $fields['enabled'] = 0;
      }

      key_save_integration_settings($fields, TRUE);
    }
  }

  drupal_set_message(t('The integration settings have been updated.'));

//  $integrations = key_get_integrations();
//
//  foreach ($integrations as $key => $integration) {
//    // If the usage was previously enabled and is now disabled.
//    if (array_key_exists($key, $original_values) && !array_key_exists($key, $values)) {
//      if ($disable_callback = ctools_plugin_get_function($integration, 'disable callback')) {
//        call_user_func($disable_callback);
//      }
//    }
//
//    // If the usage was previously disabled and is now enabled.
//    if (!array_key_exists($key, $original_values) && array_key_exists($key, $values)) {
//      if ($enable_callback = ctools_plugin_get_function($integration, 'enable callback')) {
//        call_user_func($enable_callback);
//      }
//    }
//  }
}

/**
 * Returns HTML for the Integrations form.
 *
 * @param $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 *
 * @ingroup themeable
 */
function theme_key_ui_key_integrations_fieldset($variables) {
  $form = $variables['form'];

  // Individual table headers.
  $rows = array();
  // Iterate through all the integrations, which are
  // children of this fieldset.
  foreach (element_children($form) as $key) {
    $integration = $form[$key];
    $row = array();
    unset($integration['enabled']['#title']);
    $row[] = array('class' => array('checkbox'), 'data' => drupal_render($integration['enabled']));
    $label = '<label';
    if (isset($integration['enabled']['#id'])) {
      $label .= ' for="' . $integration['enabled']['#id'] . '"';
    }
    $row[] = $label . '><strong>' . drupal_render($integration['name']) . '</strong></label>';
    $row[] = array('data' => drupal_render($integration['description']), 'class' => array('integration-description'));
    $row[] = array('data' => drupal_render($integration['module']), 'class' => array('integration-module'));

    $rows[] = $row;
  }

  return theme('table', array('header' => $form['#header'], 'rows' => $rows));
}
