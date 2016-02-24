<?php

/**
 * @file
 * Plugin definition for the generic Encryption key type.
 */

$plugin = array(
  'label' => t('Encryption'),
  'description' => t('A generic key type to use for an encryption key that does not belong to any other defined key type.'),
  'group' => 'encryption',
  'key value' => array(
    'plugin' => 'text_field'
  ),
  'default configuration' => 'key_type_encryption_default_configuration',
  'build configuration form' => 'key_type_encryption_build_configuration_form',
  'generate key value' => 'key_type_encryption_generate_key_value',
  'validate key value' => 'key_type_encryption_validate_key_value',
);

/**
 * The default plugin configuration.
 *
 * @return array
 *   The default plugin configuration.
 */
function key_type_encryption_default_configuration() {
  return array(
    'key_size' => '128',
  );
}

/**
 * Build the plugin configuration form.
 *
 * @param array $form
 * @param array $form_state
 *
 * @return array
 *   The plugin configuration form.
 */
function key_type_encryption_build_configuration_form($form, &$form_state) {
  $config = $form_state['storage']['key_config'];
  $plugin_config = $config['key_type_settings'] + key_type_encryption_default_configuration();

  $form['key_size'] = array(
    '#type' => 'select',
    '#title' => t('Key size'),
    '#description' => t('The size of the key in bits, with 8 bits per byte.'),
    '#options' => array_combine(range(32, 512, 32), range(32, 512, 32)),
    '#default_value' => $plugin_config['key_size'],
    '#required' => TRUE,
  );

  return $form;
}

/**
 * Generate a key value.
 *
 * @param array $configuration
 *
 * @return string
 *   A random key.
 */
function key_type_encryption_generate_key_value(array $configuration) {
  if (!empty($configuration['key_size'])) {
    $bytes = $configuration['key_size'] / 8;
  }
  else {
    // If no key size has been defined, use 32 bytes as the default.
    $bytes = 32;
  }
  $random_key = drupal_random_bytes($bytes);

  return $random_key;
}

/**
 * Validate the key value.
 *
 * @param $form
 * @param $form_state
 * @param $key_value
 *   The key value to validate
 */
function key_type_encryption_validate_key_value($form, &$form_state, $key_value) {
  if (empty($form_state['values']['key_size'])) {
    return;
  }

  // Validate the key size.
  $bytes = $form_state['values']['key_size'] / 8;
  if (strlen($key_value) != $bytes) {
    form_set_error('key_size', t('The selected key size does not match the actual size of the key.'));
    return;
  }
}
