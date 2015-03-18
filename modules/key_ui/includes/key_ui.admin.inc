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
    t('Storage Method'),
    t('Status'),
    array('data' => t('Operations'), 'colspan' => '2'),
  );
  $rows = array();

  foreach ($configs as $name => $config) {
    $label = $config['label'];
    $name = $config['name'];
    $description = $config['description'];
    $storage_method = key_get_storage_method($config['storage_method']);

    // Get the function to get the status of the key.
    $status_function = ctools_plugin_get_function($storage_method, 'status callback');

    // If there are any settings, use them.
    $storage_settings = isset($config['storage_settings']) ? $config['storage_settings'] : array();

    // Get the status of the key.
    $status = call_user_func($status_function, $storage_settings);

    $config_url_string = str_replace('_', '-', $name);

    $variables = array(
      'label' => $label,
      'name' => $name,
      'description' => $description,
    );

    // Set the name column.
    $row = array(theme('key_ui_key_configs_list_description', $variables));

    // Set the storage method column.
    $row[] = array('data' => $storage_method['title']);

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

  // Get all valid storage methods.
  $storage_methods = key_get_storage_methods(FALSE);

  // Get all valid storage methods as options.
  $storage_method_options = key_get_storage_methods_as_options(FALSE);

  // Determine the storage method.
  if (isset($form_state['values']['storage_method'])) {
    $storage_method = $form_state['values']['storage_method'];
  }
  elseif (isset($config['storage_method'])) {
    $storage_method = $config['storage_method'];
  }
  else {
    $storage_method = NULL;
  }

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
  $form['storage_method'] = array(
    '#type' => 'select',
    '#title' => t('Storage method'),
    '#description' => t('The method used to store the key.'),
    '#required' => TRUE,
    '#options' => $storage_method_options,
    '#default_value' => $config['storage_method'],
    '#ajax' => array(
      'method' => 'replace',
      'callback' => 'key_ui_storage_extras_ajax',
      'wrapper' => 'key-storage-extras-wrapper',
    ),
  );
  $form['storage_extras'] = array(
    '#type' => 'container',
    '#prefix' => '<div id="key-storage-extras-wrapper">',
    '#suffix' => '</div>',
  );
  $form['storage_extras']['storage_settings'] = array(
    '#type' => 'container',
    '#title' => t('Storage method settings'),
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );
  if ($storage_method && $storage_settings_form = ctools_plugin_get_function($storage_methods[$storage_method], 'settings form')) {
    $form['storage_extras']['storage_settings']['#type'] = 'fieldset';
    $form['storage_extras']['storage_settings'] += call_user_func($storage_settings_form, $config['storage_settings']);
  }

  $form['storage_extras']['storage_key'] = array(
    '#type' => 'container',
  );
  if ($storage_method && $storage_key_form = ctools_plugin_get_function($storage_methods[$storage_method], 'key form')) {
    $key = !empty($config['name']) ? key_get_key($config['name']) : '';
    $form['storage_extras']['storage_key'] += call_user_func($storage_key_form, array('key' => $key));
  }

  $form['storage_extras']['storage_instructions'] = array(
    '#type' => 'container',
    '#title' => t('Instructions'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  if ($storage_method && $storage_instructions = ctools_plugin_get_function($storage_methods[$storage_method], 'instructions')) {
    $form['storage_extras']['storage_instructions']['#type'] = 'fieldset';
    $form['storage_extras']['storage_instructions'] += call_user_func($storage_instructions);
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
function key_ui_storage_extras_ajax($form, $form_state) {
  return $form['storage_extras'];
}

/**
 * Form submission handler for key_ui_key_config_form().
 */
function key_ui_key_config_form_submit($form, &$form_state) {
  $method = key_get_storage_method($form_state['values']['storage_method']);

  foreach (array($method) as $plugin) {
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
    'storage_method' => (string) $form_state['values']['storage_method'],
    'storage_settings' => serialize($form_state['values']['storage_settings']),
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
 * Menu callback; displays the list of key usages.
 */
function key_ui_key_usage_form($form, &$form_state) {
  // Clear the plugin cache on the first page load, but not on AJAX refreshes.
  if (!isset($form_state['values'])) {
    _key_clear_plugin_cache();
  }

  $usages = key_get_usages(FALSE);
  $usage_defaults = variable_get('key_usage', array());
  $usage_default_values = array();

  foreach ($usage_defaults as $key => $value) {
    if ($value) {
      array_push($usage_default_values, $key);
    }
  }

  foreach ($usages as $key => $usage) {
    if (!in_array($key, $usage_default_values) && $usage['always_enabled']) {
      array_push($usage_default_values, $key);
    }
  }

  // Get all valid key usages as options.
  $usage_options = key_get_usages_as_options(FALSE);

  $form['usages'] = array(
    '#type' => 'checkboxes',
    '#options' => $usage_options,
    '#default_value' => $usage_default_values,
  );

  // Disable checkboxes for usages that should always be enabled.
  foreach ($usages as $key => $usage) {
    if ($usage['always_enabled']) {
      $form['usages'][$key]['#disabled'] = TRUE;
    }
  }

  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Form submission handler for key_ui_key_usage_form().
 */
function key_ui_key_usage_form_submit($form, &$form_state) {
  if (!isset($form_state['values']['usages'])) {
    return;
  }

  $usages = key_get_usages(FALSE);

  $original_values = variable_get('key_usage', array());

  $values = array();
  foreach ($form_state['values']['usages'] as $key => $value) {
    if ($value) {
      $values[$key] = 1;
    }
  }

  foreach ($usages as $key => $usage) {
    // If the usage was previously enabled and is now disabled.
    if (array_key_exists($key, $original_values) && !array_key_exists($key, $values)) {
      if ($disable_callback = ctools_plugin_get_function($usage, 'disable callback')) {
        call_user_func($disable_callback);
      }
    }

    // If the usage was previously disabled and is now enabled.
    if (!array_key_exists($key, $original_values) && array_key_exists($key, $values)) {
      if ($enable_callback = ctools_plugin_get_function($usage, 'enable callback')) {
        call_user_func($enable_callback);
      }
    }
  }

  variable_set('key_usage', $values);
}
