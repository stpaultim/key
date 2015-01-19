<?php

/**
 * @file
 * Plugin definition for the Variable key storage method.
 */

define('KEY_STORAGE_METHOD_VARIABLE_PREFIX', 'key_');

$plugin = array(
  'title' => t('Variable'),
  'description' => t('Use a variable, either from the database or from the $conf array in settings.php.'),
  'key get callback' => 'key_storage_method_variable_get_key',
  'settings form' => 'key_storage_method_variable_settings_form',
  'instructions' => 'key_storage_method_variable_instructions',
  'status callback' => 'key_storage_method_variable_status',
);

/**
 * Callback function to return a key from a variable.
 */
function key_storage_method_variable_get_key($settings) {
  if (empty($settings['variable_name'])) {
    return NULL;
  }
  $variable_name = KEY_STORAGE_METHOD_VARIABLE_PREFIX . $settings['variable_name'];
  $key = variable_get($variable_name, '');

  return $key;
}

function key_storage_method_variable_settings_form($defaults) {
  $form = array();

  $form['variable_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Variable name'),
    '#description' => t('The name of the variable in which the key will be stored. It must only contain lowercase letters, numbers, and underscores.'),
    '#default_value' => isset($defaults['variable_name']) ? $defaults['variable_name'] : '',
    '#required' => TRUE,
    '#field_prefix' => KEY_STORAGE_METHOD_VARIABLE_PREFIX,
  );

  return $form;
}

function key_storage_method_variable_instructions() {
  return array(
    'instructions' => array(
      '#markup' => t('Define the specified key in the $conf array in the site’s settings.php file. For example, if the text entered for the variable name in the storage method settings is “foobar” and the key is “12345”, add this line to settings.php: %code', array('%code' => "\$conf['key_foobar'] = '12345';")),
    )
  );
}

function key_storage_method_variable_status($settings) {
  $key = key_storage_method_variable_get_key($settings);
  if (!empty($key)) {
    $status = t('Key found');
  }
  else {
    $status = t('Key not found');
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
function _key_storage_method_variable_overridden($variable_name) {
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
