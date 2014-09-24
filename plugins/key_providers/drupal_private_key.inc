<?php

/**
 * @file
 * Plugin definition for the Drupal Private Key key provider.
 */

$plugin = array(
  'title' => t('Drupal Private Key'),
  'description' => t("Use Drupal's private key from the database."),
  'key callback' => 'key_get_drupal_private_key',
  'settings form' => 'key_drupal_private_key_settings_form',
);

/**
 * Callback method to return Drupal's private key.
 */
function key_get_drupal_private_key() {
  return drupal_get_private_key();
}

function key_drupal_private_key_settings_form() {
  $form = array();

  $form['field_one'] = array(
    '#type' => 'textfield',
    '#title' => t('A miscellaneous text file!'),
    '#description' => t('This is some different description text so we can work with this stuff.'),
    '#default_value' => isset($defaults['field_one']) ? $defaults['field_one'] : '',
//    '#element_validate' => array('encrypt_file_key_path_validate'),
    '#required' => TRUE,
  );
  $form['field_two'] = array(
    '#type' => 'select',
    '#title' => t('Method'),
    '#options' => array(
      'option_1' => t('First Option'),
      'option_2' => t('Second Option'),
    ),
    '#default_value' => isset($defaults['field_two']) ? $defaults['field_two'] : '',
  );

  return $form;
}