<?php

/**
 * @file
 * Plugin definition for the Authorize.net payment method.
 */

$plugin = array(
  'title' => t('Authorize.net'),
  'description' => t('Integration with the Authorize.net payment method.'),
  'type' => 'commerce_payment',
  'settings' => array(
    'method_id' => 'authnet_aim',
    'key_fields' => array('tran_key'),
  ),
);
