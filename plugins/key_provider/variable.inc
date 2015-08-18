<?php

/**
 * @file
 * Plugin definition for the Variable key provider.
 */

define('KEY_PROVIDER_VARIABLE_PREFIX', 'key_');

$plugin = array(
  'title' => t('Variable'),
  'description' => t('Use a variable, either from the database or from the $conf array in settings.php.'),
  'key get callback' => 'key_provider_variable_get_key',
  'key set callback' => 'key_provider_variable_set_key',
  'settings form' => 'key_provider_variable_settings_form',
  'key form' => 'key_provider_variable_key_form',
  'instructions' => 'key_provider_variable_instructions',
  'status callback' => 'key_provider_variable_status',
);

/**
 * Callback function to return a key from a variable.
 */
function key_provider_variable_get_key($settings) {
  if (empty($settings['variable_name'])) {
    return NULL;
  }
  $variable_name = KEY_PROVIDER_VARIABLE_PREFIX . $settings['variable_name'];
  $key = variable_get($variable_name, '');

  return $key;
}

/**
 * Callback function to set a key from a variable.
 */
function key_provider_variable_set_key($settings, $key) {
  if (empty($settings['variable_name'])) {
    return NULL;
  }
  $variable_name = KEY_PROVIDER_VARIABLE_PREFIX . $settings['variable_name'];
  if (empty($key)) {
    variable_del($variable_name);
  }
  else {
    variable_set($variable_name, $key);
  }
}

function key_provider_variable_settings_form($defaults) {
  $form = array();

  $form['variable_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Variable name'),
    '#description' => t('The name of the variable in which the key will be stored. It must only contain lowercase letters, numbers, and underscores.'),
    '#default_value' => isset($defaults['variable_name']) ? $defaults['variable_name'] : '',
    '#required' => TRUE,
    '#field_prefix' => KEY_PROVIDER_VARIABLE_PREFIX,
  );

  return $form;
}

function key_provider_variable_key_form($defaults) {
  $form = array();

  $form['key'] = array(
    '#type' => 'textarea',
    '#title' => t('Key value'),
    '#description' => t('Enter the key to save it to the database. If it’s defined in the settings.php file, as described in the instructions, this field should be left blank.'),
    '#default_value' => isset($defaults['key']) ? $defaults['key'] : '',
  );

  return $form;
}

function key_provider_variable_instructions() {
  return array(
    'instructions' => array(
      '#markup' => t('To set the key in the site’s settings.php file, leave the key value blank on this form and define the specified key in the $conf array in settings.php. For example, if the text entered for the variable name in the key provider settings is “foobar” and the key is “12345”, add this line to settings.php: %code', array('%code' => "\$conf['key_foobar'] = '12345';")),
    )
  );
}

function key_provider_variable_status($settings) {
  $key = key_provider_variable_get_key($settings);
  if (!empty($key)) {
    $status = array(
      'valid' => KEY_STATUS_VALID,
      'message' => t('Key found'),
    );
  }
  else {
    $status = array(
      'valid' => KEY_STATUS_NOT_VALID,
      'message' => t('Key not found'),
    );
  }

  return $status;
}

/**
 * Helper function to determine if a variable is overridden.
 *
 * @param $variable_name
 *   The variable to be tested.
 *
 * @return bool
 *   A flag indicating whether the variable is overridden.
 */
function _key_provider_variable_overridden($variable_name) {
  $variable_overridden = FALSE;
  $key = variable_get($variable_name, '');
  variable_del($variable_name);
  if (variable_get($variable_name, '')) {
    $variable_overridden = TRUE;
  }
  else {
    variable_set($variable_name, $key);
  }

  return $variable_overridden;
}
