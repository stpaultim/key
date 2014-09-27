<?php

/**
 * @file
 * Plugin definition for the Variable key provider.
 */

$plugin = array(
  'title' => t('Variable'),
  'description' => t('Use a variable, either from the database or from the $conf array in settings.php.'),
  'key callback' => 'key_provider_variable_get_key',
  'settings form' => 'key_provider_variable_settings_form',
);

/**
 * Callback function to return a variable.
 */
function key_provider_variable_get_key() {
  $key = drupal_get_private_key();

  return $key;
}

function key_provider_variable_settings_form($defaults) {
  $form = array();

  $form['use_private_key'] = array(
    '#type' => 'checkbox',
    '#title' => t("Use Private Key"),
    '#description' => t("If checked, this site's Drupal Private Key will be used."),
    '#default_value' => isset($defaults['use_private_key']) ? $defaults['use_private_key'] : FALSE,
  );

  return $form;
}
