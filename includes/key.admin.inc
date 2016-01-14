<?php

/**
 * @file
 * Administrative functionality for managing key configurations.
 */

/**
 * Menu callback; displays the list of key configurations.
 */
function key_configs_list() {
  $configs = key_get_configs();

  $header = array(
    t('Name'),
    t('Provider'),
    array('data' => t('Operations'), 'colspan' => '2'),
  );
  $rows = array();

  foreach ($configs as $id => $config) {
    $label = $config['label'];
    $id = $config['id'];
    $description = $config['description'];
    $provider = key_get_plugin('key_provider', $config['key_provider']);

    $config_url_string = str_replace('_', '-', $id);

    $variables = array(
      'label' => $label,
      'id' => $id,
      'description' => $description,
    );

    // Set the name column.
    $row = array(theme('key_configs_list_description', $variables));

    // Set the key provider column.
    $row[] = array('data' => $provider['label']);

    // Set the edit column.
    $row[] = array('data' => l(t('edit'), KEY_MENU_PATH . '/manage/' . $config_url_string));

    // Set the delete column.
    $row[] = array('data' => l(t('delete'), KEY_MENU_PATH . '/manage/' . $config_url_string . '/delete'));

    $rows[] = $row;
  }

  $build['key_configs_list_table'] = array(
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
function key_config_form($form, &$form_state, $config = NULL) {
  // Only when the form is first built.
  if (!$form_state['rebuild']) {
    // Clear the plugin cache.
    _key_clear_plugin_cache();

    // Store the original key configuration.
    $form_state['#original_key'] = $config;

    // Store the key value data.
    $form_state['#key_value'] = _key_key_value_data($config);
  }

  // Get all valid key types.
  $key_types = key_get_plugins('key_type', FALSE);

  // Get all valid key types as options.
  $key_type_options = key_get_plugins_as_options('key_type', FALSE);

  // Get all valid key providers.
  $providers = key_get_plugins('key_provider', FALSE);

  // Get all valid key providers as options.
  $provider_options = key_get_plugins_as_options('key_provider', FALSE);

  // Store the original key in the form.
  if (!empty($config['id'])) {
    $key_value = key_get_key($config['id']);

    $form['#original_key'] = array(
      'config' => $config,
      'key_value' => $key_value,
    );

    if (!empty($key_value) && $key_value_obscure = ctools_plugin_get_function($providers[$config['key_provider']], 'key value obscure')) {
      $form['#original_key']['key_value_obscured'] = call_user_func($key_value_obscure, $key_value, $config);
    }
  }

  // Determine the current key type.
  if (isset($form_state['values']['key_type'])) {
    $key_type = $form_state['values']['key_type'];
  }
  elseif (isset($config['key_type'])) {
    $key_type = $config['key_type'];
  }
  else {
    $key_type = 'basic';
  }

  // Determine the current key provider.
  if (isset($form_state['values']['key_provider'])) {
    $provider = $form_state['values']['key_provider'];
  }
  elseif (isset($config['key_provider'])) {
    $provider = $config['key_provider'];
  }
  else {
    $provider = 'configuration';
  }

  $form['label'] = array(
    '#title' => t('Key name'),
    '#type' => 'textfield',
    '#default_value' => $config['label'],
    '#required' => TRUE,
    '#size' => 30,
  );
  $form['id'] = array(
    '#type' => 'machine_name',
    '#default_value' => $config['id'],
    '#maxlength' => 32,
    '#disabled' => isset($config['id']),
    '#machine_name' => array(
      'exists' => 'key_config_load',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for the key. It must only contain lowercase letters, numbers, and underscores.'),
  );
  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textfield',
    '#default_value' => $config['description'],
    '#description' => t('A short description of the key.'),
  );

  // This is the element that contains all of the dynamic parts of the form.
  $form['settings'] = array(
    '#type' => 'container',
    '#prefix' => '<div id="key-settings">',
    '#suffix' => '</div>',
  );

  // Key type section.
  $form['settings']['type_section'] = array(
    '#type' => 'fieldset',
    '#title' => t('Type settings'),
    '#collapsible' => TRUE,
  );
  $form['settings']['type_section']['key_type'] = array(
    '#type' => 'select',
    '#title' => t('Key type'),
    '#options' => $key_type_options,
    '#required' => TRUE,
    '#default_value' => isset($config['key_type']) ? $config['key_type'] : 'basic',
    '#ajax' => array(
      'callback' => '_key_config_form_ajax_update_settings',
      'event' => 'change',
      'wrapper' => 'key-settings',
    ),
  );
  $form['settings']['type_section']['key_type_settings'] = array(
    '#type' => 'container',
    '#title' => t('Key type settings'),
    '#title_display' => FALSE,
    '#tree' => TRUE,
  );
  if ($key_type) {
    // Add any additional key type settings.
    if ($key_type_settings_form = ctools_plugin_get_function($key_types[$key_type], 'build configuration form')) {
      $form['settings']['type_section']['key_type_settings'] += call_user_func($key_type_settings_form, $config['key_type_settings']);
    }
  }

  // Key provider section.
  $form['settings']['provider_section'] = array(
    '#type' => 'fieldset',
    '#title' => t('Provider settings'),
    '#collapsible' => TRUE,
  );
  $form['settings']['provider_section']['key_provider'] = array(
    '#type' => 'select',
    '#title' => t('Key provider'),
    '#options' => $provider_options,
    '#required' => TRUE,
    '#default_value' => isset($config['key_provider']) ? $config['key_provider'] : 'configuration',
    '#ajax' => array(
      'callback' => '_key_config_form_ajax_update_settings',
      'event' => 'change',
      'wrapper' => 'key-settings',
    ),
  );
  $form['settings']['provider_section']['key_provider_settings'] = array(
    '#type' => 'container',
    '#title' => t('Key provider settings'),
    '#title_display' => FALSE,
    '#tree' => TRUE,
  );

  $form['settings']['provider_section']['key_value'] = array(
    '#type' => 'container',
    '#title' => t('Key value'),
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );

  // Unset the input values for provider settings and key value,
  // so they can be updated via AJAX.
  unset($form_state['input']['key_provider_settings']);
  unset($form_state['input']['key_value']);

  if ($provider) {
    // Add the Base64 checkbox to the settings, if allowed.
    if ($providers[$provider]['allow base64 encoding']) {
      $form['settings']['provider_section']['key_provider_settings']['base64_encoded'] = array(
        '#type' => 'checkbox',
        '#title' => t('Base64-encoded'),
        '#description' => t('Check this if the key has been Base64-encoded. If the key should be used as-is, without Base64-decoding, leave this unchecked.'),
        '#default_value' => isset($config['key_provider_settings']['base64_encoded']) ? $config['key_provider_settings']['base64_encoded'] : 0,
      );
    }

    // Add any additional provider settings.
    if ($provider_settings_form = ctools_plugin_get_function($providers[$provider], 'build configuration form')) {
      $form['settings']['provider_section']['key_provider_settings'] += call_user_func($provider_settings_form, $config['key_provider_settings']);
    }

    // Add the value form, if defined.
    if ($provider && $key_value_form = ctools_plugin_get_function($providers[$provider], 'key value form')) {
      $form['settings']['provider_section']['key_value']['#type'] = 'fieldset';

      // Determine what to display for the key value, if anything.
      if (isset($form['#original_key']['key_value_obscured'])) {
        $key_value = ($provider == $config['key_provider']) ? $form['#original_key']['key_value_obscured'] : '';
      }
      else {
        $key_value = (isset($form['#original_key']['key_value'])) ? $form['#original_key']['key_value'] : '';
      }

      $form['settings']['provider_section']['key_value'] += call_user_func($key_value_form, array('key_value' => $key_value));
    }
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
    '#submit' => array('key_config_form_submit'),
    '#weight' => 40,
  );
  if (isset($config['id'])) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete key'),
      '#submit' => array('key_config_form_delete_submit'),
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
 * Callback for AJAX form re-rendering.
 */
function _key_config_form_ajax_update_settings($form, $form_state) {
  return $form['settings'];
}

/**
 * Form validate handler for key_config_form().
 */
function key_config_form_validate($form, &$form_state) {
  $provider = key_get_plugin('key_provider', $form_state['values']['key_provider']);

  if ($validate_callback = ctools_plugin_get_function($provider, 'validate configuration form')) {
    call_user_func($validate_callback, $form, $form_state);
  }
}

/**
 * Form submission handler for key_config_form().
 */
function key_config_form_submit($form, &$form_state) {
  $provider = key_get_plugin('key_provider', $form_state['values']['key_provider']);

  if ($submit_callback = ctools_plugin_get_function($provider, 'submit configuration form')) {
    call_user_func_array($submit_callback, array($form, &$form_state));
  }

  // If the provider has changed, call the delete callback for the old
  // provider.
  if (isset($form['#original_key']['config'])) {
    $original_config = $form['#original_key']['config'];
    $original_provider = key_get_plugin('key_provider', $original_config['key_provider']);
    if (($provider['name'] != $original_provider['name']) && ($delete_callback = ctools_plugin_get_function($original_provider, 'submit delete form'))) {
      call_user_func($delete_callback, $original_config);
    }
  }

  $fields = array(
    'id' => (string) $form_state['values']['id'],
    'label' => (string) $form_state['values']['label'],
    'description' => (string) $form_state['values']['description'],
    'key_provider' => (string) $form_state['values']['key_provider'],
    'key_provider_settings' => (isset($form_state['values']['key_provider_settings'])) ? serialize($form_state['values']['key_provider_settings']) : '',
    'created' => (string) $form_state['values']['created'],
  );

  key_save_config($fields);

  $form_state['redirect'] = KEY_MENU_PATH . '/list';
}

/**
 * Form submission handler for key_config_form().
 *
 * Handles the 'Delete' button on the key configuration form.
 */
function key_config_form_delete_submit($form, &$form_state) {
  $form_state['redirect'] = KEY_MENU_PATH . '/manage/' . str_replace('_', '-', $form['id']['#default_value']) . '/delete';
}

/**
 * Menu callback to delete a key configuration.
 */
function key_config_delete_confirm($form, &$form_state, $config) {
  $form['id'] = array('#type' => 'value', '#value' => $config['id']);
  $form['label'] = array('#type' => 'value', '#value' => $config['label']);

  $message = t('Are you sure you want to delete the key %label?', array('%label' => $config['label']));
  $caption = '<p>' . t('This action cannot be undone.') . '</p>';

  return confirm_form($form, filter_xss_admin($message), KEY_MENU_PATH, filter_xss_admin($caption), t('Delete'));
}

/**
 * Submit handler for key_config_delete_confirm().
 */
function key_config_delete_confirm_submit($form, &$form_state) {
  $config = key_get_config($form_state['values']['id']);
  $provider = key_get_plugin('key_provider', $config['key_provider']);

  if ($delete_callback = ctools_plugin_get_function($provider, 'submit delete form')) {
    call_user_func($delete_callback, $config);
  }

  db_delete('key_config')
    ->condition('id', $form_state['values']['id'])
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
 *   - id: The machine name of the configuration.
 *   - description: A brief description of the configuration.
 *
 * @ingroup themeable
 */
function theme_key_configs_list_description($variables) {
  $label = $variables['label'];
  $id = $variables['id'];
  $description = $variables['description'];

  $output = check_plain($label);
  $output .= ' <small>' . t('(Machine name: @id)', array('@id' => $id)) . '</small>';
  $output .= '<div class="description">' . filter_xss_admin($description) . '</div>';

  return $output;
}

function _key_key_value_data($config = NULL) {
  $key_value_data = array(
    'original' => NULL,
    'processed_original' => NULL,
    'obscured' => NULL,
    'current' => '',
  );

  // If this is a new key being added, return the defaults.
  if (!$config) {
    return $key_value_data;
  }

  // Determine the obscure options.
  $obscure_options = [];
  $obscure_options['key_provider_id'] = $config['key_provider'];

  return $key_value_data;
}
