<?php

/**
 * @file
 * Plugin definition for the Drupal Private Key key provider.
 */

$plugin = array(
  'title' => t('Drupal key_drupal_variable_key variable'),
  'description' => t('Use a variable called key_drupal_variable_key, preferably set in your settings.php $conf array.'),
  'key callback' => 'key_get_key_drupal_variable_key',
  'settings form' => NULL,
  'dependency callback' => '_key_drupal_variable_is_present',
);

/**
 * Callback function to return a variable.
 */
function key_get_key_drupal_variable_key() {
  $key = variable_get('key_drupal_variable_key', NULL);
  dpm($key);
  if (empty($key)) {
    watchdog('key', 'You need to set the key_drupal_variable_key variable, preferably in $conf in your settings.php.', array(), WATCHDOG_EMERGENCY);
    drupal_set_message("Key settings are insufficient. See your site log for more information.");
  }
  return $key;
}

/**
 * Callback function to validate the variable is present.
 */
function _key_drupal_variable_is_present() {
  $errors = array();

  $key = variable_get('key_drupal_variable_key', NULL);
  dpm($key);
  if (empty($key)) {
    $errors[] = t('The key_drupal_variable_key is currently null. You should set it, preferably in $conf in your settings.php.');
  }

  return $errors;
}
