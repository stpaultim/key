<?php

/**
 * @file
 * Plugin definition for the PayLeap Card on File payment method.
 */

$plugin = array(
  'title' => t('PayLeap Card on File'),
  'description' => t('Integration with the PayLeap Card on File payment method.'),
  'type' => 'commerce_payment',
  'settings' => array(
    'method_id' => 'payleap_cof',
    'key_fields' => array('tran_key'),
  ),
);
