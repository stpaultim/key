<?php

/**
 * @file
 * Plugin definition for the File key provider.
 */

$plugin = array(
  'title' => t('File'),
  'description' => t('Use a file, preferably one that is outside the web root directory.'),
  'key get callback' => 'key_provider_file_get_key',
  'settings form' => 'key_provider_file_settings_form',
);

/**
 * Callback function to return a key from a file.
 */
function key_provider_file_get_key($settings) {
  if (empty($settings['location']) || empty($settings['provider'])) {
    return NULL;
  }
  $file = $settings['location'];

  // Make sure the file exists and is readable.
  if (!is_file($file) || !is_readable($file)) {
    return NULL;
  }

  $key = file_get_contents($file);

  return $key;
}

/**
 * Settings form for the File key provider.
 */
function key_provider_file_settings_form($defaults) {
  $form = array();

  $form['location'] = array(
    '#type' => 'textfield',
    '#title' => t('File location'),
    '#description' => t('The location of the file in which the key will be stored. The path may be absolute (e.g., %abs), relative to the Drupal directory (e.g., %rel), or defined using a stream wrapper (e.g., %str).', array(
      '%abs' => '/etc/keys/foobar.key',
      '%rel' => '../keys/foobar.key',
      '%str' => 'private://keys/foobar.key',
      )
    ),
    '#default_value' => isset($defaults['location']) ? $defaults['location'] : '',
    '#required' => TRUE,
  );

  return $form;
}
