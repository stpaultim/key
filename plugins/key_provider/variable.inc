<?php

/**
 * @file
 * Plugin definition for the Variable key provider.
 */

define('KEY_PROVIDER_VARIABLE_PREFIX', 'key_');

$plugin = array(
  'title' => t('Variable'),
  'description' => t('Use a variable, either from the database or from the $conf array in settings.php.'),
  'key get value' => 'key_provider_variable_get_key',
  'config form submit' => 'key_provider_variable_config_form_submit',
  'provider settings form' => 'key_provider_variable_settings_form',
  'key value form' => 'key_provider_variable_key_value_form',
);

/**
 * Return a key from a variable.
 */
function key_provider_variable_get_key($config) {
  if (empty($config['provider_settings']['variable_name'])) {
    return NULL;
  }
  $variable_name = KEY_PROVIDER_VARIABLE_PREFIX . $config['provider_settings']['variable_name'];
  $key = variable_get($variable_name, '');

  return $key;
}

/**
 * Submit callback.
 */
function key_provider_variable_config_form_submit($form, $form_state) {
  if (empty($form_state['values']['provider_settings']['variable_name'])) {
    return NULL;
  }
  $variable_name = KEY_PROVIDER_VARIABLE_PREFIX . $form_state['values']['provider_settings']['variable_name'];
  $key = (isset($form_state['values']['key_value']['key_text'])) ? $form_state['values']['key_value']['key_text'] : '';

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

function key_provider_variable_key_value_form($defaults) {
  $form = array();

  $form['key_text'] = array(
    '#type' => 'textarea',
    '#title' => t('Key value'),
    '#description' => t('Enter the key to save it to the database. If it’s defined in the settings.php file, this field should be left blank.'),
    '#default_value' => isset($defaults['key']) ? $defaults['key'] : '',
  );

  return $form;
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
