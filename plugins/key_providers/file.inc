<?php

/**
 * @file
 * Plugin definition for the File key provider.
 */

function _key_get_name() {
  $name = 'key';
  ctools_include('object-cache');
  $key_module_name = ctools_object_cache_get('key_name', 'key_name');

  if (isset($key_module_name)) {
    $name = $key_module_name;
  }

  return $name . '_key.key';
}


$plugin = array(
  'title' => t('File'),
  'description' => t('Use a file to store the key.'),
  'key callback' => 'key_get_file_key',
  'settings form' => 'key_file_key_settings_form',
  'submit callback' => 'key_file_key_settings_form_submit',
  'static key' => TRUE,
);

/**
 * Callback method to return the key from a file.
 */
function key_get_file_key($settings) {
  $file_name = '';

  if (isset($settings['path'])) {
    $file_name = $settings['path'] . '/' . _key_get_name();
  }

  // If the file doesn't exist, just abort.
  if (!file_exists($file_name)) {
    drupal_set_message(t('The file %file does not exist! The key cannot be retrieved.', array('%file' => $file_name)), 'error');
    return;
  }

  switch ($settings['method']) {
    case 'file_contents':
      return file_get_contents($file_name);

    case 'md5':
      return md5_file($file_name);

  }
}

/**
 * Settings form for our key provider.
 */
function key_file_key_settings_form($defaults) {
  $form = array();

  $form['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path to Key File'),
    '#description' => t('Filepath may be absolute (e.g. %abs) or relative to the Drupal directory (e.g. %rel).', array(
      '%abs' => '/var/www',
      '%rel' => '../../keys',
    )),
    '#default_value' => isset($defaults['path']) ? $defaults['path'] : '',
    '#element_validate' => array('key_file_key_path_validate'),
    '#required' => TRUE,
    '#field_suffix' => '/' . _key_get_name(),
  );
  $form['method'] = array(
    '#type' => 'select',
    '#title' => t('Method'),
    '#options' => array(
      'file_contents' => t('File Contents'),
      'md5' => t('MD5'),
    ),
    '#default_value' => isset($defaults['method']) ? $defaults['method'] : 'file_contents',
  );

  return $form;
}

/**
 * Validate key path.
 */
function key_file_key_path_validate($element, &$form_state, $form) {
  $directory = $element['#value'];
  $file = $element['#value'] . '/' . _key_get_name();

  if (!file_exists($directory)) {
    form_error($element, t('The directory %dir does not exist.', array('%dir' => $directory)));
  }
  elseif (!is_dir($directory)) {
    form_error($element, t('The key path provided exists, but is not a directory.'));
  }
  elseif (!file_exists($file) && !is_writable($directory)) {
    form_error($element, t('The directory %dir needs to be writable in order to create the key file.', array(
      '%dir' => $directory,
    )));
  }
  elseif (file_exists($file) && !is_readable($file)) {
    form_error($element, t('The key file %file exists, but is not readable by the web server.', array(
      '%file' => $file,
    )));
  }
}

/**
 * Submit callback for the settings form.
 */
function key_file_key_settings_form_submit($form, $form_state) {
  $directory = $form_state['values']['key_key_providers_file_settings']['path'];
  $file = $directory . '/' . _key_get_name();

  // Check for contents of file; do not write over the key.
  if (file_exists($file) && file_get_contents($file)) {
    return;
  }

  // Create a default key.
  $open_file = fopen($file, 'wb');
  $new_key = md5(uniqid(mt_rand(0, mt_rand()), TRUE)) . md5(uniqid(mt_rand(0, mt_rand()), TRUE));
  fwrite($open_file, $new_key);
  fclose($open_file);

  // Ensure that the file is only readable and writable by owner.
  drupal_chmod($file, 0600);

  drupal_set_message(t('The key file has been written to %file', array(
    '%file' => $file,
  )));
}
