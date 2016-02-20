<?php

/**
 * @file
 * Plugin definition for the AES Encryption key type.
 */

// Use the generic Encryption key type as a base.
module_load_include('inc', 'key', 'plugins/key_type/encryption');

$plugin = array(
  'label' => t('AES Encryption'),
  'description' => t('Used for encrypting and decrypting data with the Advanced Encryption Standard (AES) cipher.'),
  'group' => 'encryption',
  'key value' => array(
    'plugin' => 'text_field'
  ),
  'default configuration' => 'key_type_encryption_default_configuration',
  'build configuration form' => 'key_type_aes_encryption_build_configuration_form',
  'generate key value' => 'key_type_encryption_generate_key_value',
  'validate key value' => 'key_type_encryption_validate_key_value',
);

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
  $form = key_type_encryption_build_configuration_form($form, $form_state);

  // Restrict the key sizes to the ones that AES supports.
  $form['key_size']['#options'] = drupal_map_assoc([128, 192, 256]);

  return $form;
}
