<?php

/**
 * @file
 * Plugin definition for the AES Encryption key type.
 */

$plugin = array(
  'label' => t('AES Encryption'),
  'description' => t('Used for encrypting and decrypting data with the Advanced Encryption Standard (AES) cipher.'),
  'group' => 'encryption',
  'key value' => array(
    'plugin' => 'text_field'
  ),
  'default configuration' => 'key_type_aes_encryption_default_configuration',
  'build configuration form' => 'key_type_aes_encryption_build_configuration_form',
  'generate key value' => 'key_type_aes_encryption_generate_key_value',
  'validate key value' => 'key_type_aes_encryption_validate_key_value',
);

/**
 * The default plugin configuration.
 *
 * @return array
 *   The default plugin configuration.
 */
function key_type_aes_encryption_default_configuration() {
  return array(
    'key_size' => '128_bits',
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
function key_type_aes_encryption_build_configuration_form($form, &$form_state) {
  $config = $form_state['storage']['key_config'];
  $plugin_config = $config['key_type_settings'] + key_type_aes_encryption_default_configuration();

  $form['key_size'] = array(
    '#type' => 'select',
    '#title' => t('Key size'),
    '#description' => t('The size of the key in bits. 128 bits is 16 bytes.'),
    '#options' => array(
      '128_bits' => t('128 bits'),
      '192_bits' => t('192 bits'),
      '256_bits' => t('256 bits'),
    ),
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
function key_type_aes_encryption_generate_key_value(array $configuration) {
  $bytes = drupal_substr($configuration['key_size'], 0, 3) / 8;
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
function key_type_aes_encryption_validate_key_value($form, &$form_state, $key_value) {
  // Validate the key size.
  $bytes = drupal_substr($form_state['values']['key_size'], 0, 3) / 8;
  if (drupal_strlen($key_value) != $bytes) {
    form_set_error('key_size', t('The selected key size does not match the actual size of the key.'));
    return;
  }
}
