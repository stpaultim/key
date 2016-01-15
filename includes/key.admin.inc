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
    t('Key'),
    t('Type'),
    t('Provider'),
    array('data' => t('Operations'), 'colspan' => '2'),
  );
  $rows = array();

  foreach ($configs as $id => $config) {
    $label = $config['label'];
    $id = $config['id'];
    $description = $config['description'];
    $key_type = key_get_plugin('key_type', $config['key_type']);
    $key_provider = key_get_plugin('key_provider', $config['key_provider']);

    $config_url_string = str_replace('_', '-', $id);

    $variables = array(
      'label' => $label,
      'id' => $id,
      'description' => $description,
    );

    // Set the name column.
    $row = array(theme('key_configs_list_description', $variables));

    // Set the key type column.
    $row[] = array('data' => $key_type['label']);

    // Set the key provider column.
    $row[] = array('data' => $key_provider['label']);

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
 * @param array $form
 * @param array $form_state
 * @param array $config
 *   (optional) An array representing the configuration, when editing an
 *   existing configuration.
 *
 * @return array
 *   The form.
 */
function key_config_form($form, &$form_state, $config = array()) {
  // Only when the form is first built.
  if (!$form_state['rebuild']) {
    // Clear the plugin cache.
    _key_clear_plugin_cache();

    // Store the original key configuration.
    $form_state['original_key'] = $config;

    // Store the key value data.
    $form_state['key_value'] = _key_key_value_data($config);

    // If this is a new key, set the defaults.
    if (empty($config)) {
      $config = _key_config_defaults();
    }
  }

  // Only if the form is rebuilding.
  if ($form_state['rebuild']) {
    // If the key type is responsible for the rebuild, update the
    // key type settings.
    if ($form_state['triggering_element']['#name'] == 'key_type') {
      _key_update_plugin_settings('key_type', $form_state);
    }

    // If the key provider is responsible for the rebuild, update the
    // key provider settings.
    if ($form_state['triggering_element']['#name'] == 'key_provider') {
      _key_update_plugin_settings('key_provider', $form_state);
    }

    dpm($form_state);

    // Update the configuration with the current form values.
    $fields = array('label', 'id', 'description');
    foreach ($fields as $field) {
      if (isset($form_state['values'][$field])) {
        $config[$field] = $form_state['values'][$field];
      }
    }

    foreach (_key_get_plugin_types() as $type) {
      if (isset($form_state['values'][$type])) {
        $config[$type] = $form_state['values'][$type];
      }
      if (isset($form_state['values'][$type . '_settings'])) {
        $config[$type . '_settings'] = $form_state['values'][$type . '_settings'];
      }
      else {
        $config[$type . '_settings'] = array();
      }
    }

    dpm($config);
  }

//  // Load all valid plugins.
//  $plugins = array();
//  foreach (_key_get_plugin_types() as $type) {
//    $plugins[$type] = key_get_plugins($type, FALSE);
//  }
//
//  $plugins['key_type'] = key_get_plugins();
//
//  // Load the plugins that belong to this key configuration.
//
//  dpm($plugins);
//
//
//
//  // Get all valid key type plugins.
//  $key_types = key_get_plugins('key_type', FALSE);
//
//  // Get all valid key providers.
//  $key_providers = key_get_plugins('key_provider', FALSE);


//  // Store the original key in the form.
//  if (!empty($config['id'])) {
//    $key_value = key_get_key($config['id']);
//
//    $form['#original_key'] = array(
//      'config' => $config,
//      'key_value' => $key_value,
//    );
//
//    if (!empty($key_value) && $key_value_obscure = ctools_plugin_get_function($key_providers[$config['key_provider']], 'key value obscure')) {
//      $form['#original_key']['key_value_obscured'] = call_user_func($key_value_obscure, $key_value, $config);
//    }
//  }

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
    '#disabled' => !empty($config['id']),
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
    '#options' => key_get_plugins_as_options('key_type', FALSE),
    '#required' => TRUE,
    '#default_value' => $config['key_type'],
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

  // Add any additional key type settings.
  if ($key_type_settings_form = ctools_plugin_get_function(key_get_plugin('key_type', $config['key_type']), 'build configuration form')) {
    $form['settings']['type_section']['key_type_settings'] += call_user_func($key_type_settings_form, $config['key_type_settings']);
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
    '#options' => key_get_plugins_as_options('key_provider', FALSE),
    '#required' => TRUE,
    '#default_value' => $config['key_provider'],
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

  // Add any additional provider settings.
  if ($provider_settings_form = ctools_plugin_get_function(key_get_plugin('key_provider', $config['key_provider']), 'build configuration form')) {
    $plugin_form_state = _key_create_plugin_form_state('key_provider', $form_state);
    $form['settings']['provider_section']['key_provider_settings'] += call_user_func($provider_settings_form, array(), $plugin_form_state);
  }

//  $form['settings']['provider_section']['key_value'] = array(
//    '#type' => 'container',
//    '#title' => t('Key value'),
//    '#collapsible' => TRUE,
//    '#tree' => TRUE,
//  );

  // Unset the input values for provider settings and key value,
  // so they can be updated via AJAX.
//  unset($form_state['input']['key_provider_settings']);
//  unset($form_state['input']['key_value']);
//
//  if ($provider) {
//    // Add the Base64 checkbox to the settings, if allowed.
//    if ($key_providers[$provider]['allow base64 encoding']) {
//      $form['settings']['provider_section']['key_provider_settings']['base64_encoded'] = array(
//        '#type' => 'checkbox',
//        '#title' => t('Base64-encoded'),
//        '#description' => t('Check this if the key has been Base64-encoded. If the key should be used as-is, without Base64-decoding, leave this unchecked.'),
//        '#default_value' => isset($config['key_provider_settings']['base64_encoded']) ? $config['key_provider_settings']['base64_encoded'] : 0,
//      );
//    }
//
//    // Add any additional provider settings.
//    if ($provider_settings_form = ctools_plugin_get_function($key_providers[$provider], 'build configuration form')) {
//      $form['settings']['provider_section']['key_provider_settings'] += call_user_func($provider_settings_form, $config['key_provider_settings']);
//    }
//
//    // Add the value form, if defined.
//    if ($provider && $key_value_form = ctools_plugin_get_function($key_providers[$provider], 'key value form')) {
//      $form['settings']['provider_section']['key_value']['#type'] = 'fieldset';
//
//      // Determine what to display for the key value, if anything.
//      if (isset($form['#original_key']['key_value_obscured'])) {
//        $key_value = ($provider == $config['key_provider']) ? $form['#original_key']['key_value_obscured'] : '';
//      }
//      else {
//        $key_value = (isset($form['#original_key']['key_value'])) ? $form['#original_key']['key_value'] : '';
//      }
//
//      $form['settings']['provider_section']['key_value'] += call_user_func($key_value_form, array('key_value' => $key_value));
//    }
//  }

//  $form['created'] = array(
//    '#type' => 'value',
//    '#value' => $config['created'],
//  );

  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save key'),
    '#submit' => array('key_config_form_submit'),
    '#weight' => 40,
  );
  if (!empty($config['id'])) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete key'),
      '#submit' => array('key_config_form_delete_submit'),
      '#limit_validation_errors' => array(),
      '#weight' => 45,
    );
  }

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
  if (!$form_state['submitted']) {
    return;
  }

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
    'key_type' => (string) $form_state['values']['key_type'],
    'key_type_settings' => (isset($form_state['values']['key_type_settings'])) ? serialize($form_state['values']['key_type_settings']) : array(),
    'key_provider' => (string) $form_state['values']['key_provider'],
    'key_provider_settings' => (isset($form_state['values']['key_provider_settings'])) ? serialize($form_state['values']['key_provider_settings']) : array(),
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
  $id = (isset($form_state['values']['id'])) ? $form_state['values']['id'] : NULL;
  key_delete_config($id);
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

/**
 * Update plugin settings.
 */
function _key_update_plugin_settings($type, &$form_state) {
  $original_key = $form_state['original_key'];
  $current_plugin_id = $form_state['values'][$type];

  // If an original key exists and the plugin ID matches the current one.
  if ($original_key && ($original_key[$type] == $current_plugin_id)) {
    // Use the settings from the original key's plugin.
    $settings = $original_key[$type . '_settings'];
  }
  else {
    // Reset the settings for the plugin to the defaults.
    $plugin = key_get_plugin($type, $current_plugin_id);
    $plugin_defaults = ($plugin_defaults_function = ctools_plugin_get_function($plugin, 'default configuration')) ? call_user_func($plugin_defaults_function) : array();
    $settings = $plugin_defaults;
  }

  $form_state['values'][$type . '_settings'] = $settings;
}

/**
 * Creates a form state for a plugin.
 *
 * @param string $type
 *   The plugin type ID.
 * @param array $form_state
 *   The form state to copy values from.
 *
 * @return array
 *   A copy of the form state with values from the plugin.
 */
function _key_create_plugin_form_state($type, $form_state) {
  // Copy the form state.
  $plugin_form_state = $form_state;

  // Clear the values, except for this plugin type's settings.
  $plugin_form_state['values'] = (isset($form_state['values']['$type' . '_settings'])) ? $form_state['values']['$type' . '_settings'] : array();

  return $plugin_form_state;
}
