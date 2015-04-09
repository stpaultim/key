<?php

/**
 * @file
 * Plugin definition for the PayLeap Direct Processing payment method.
 */

$plugin = array(
  'title' => t('PayLeap Direct Processing'),
  'description' => t('Integration with the PayLeap Direct Processing payment method.'),
  'type' => 'commerce_payment',
  'settings' => array(
    'method_id' => 'payleap_direct',
    'key_fields' => array('tran_key'),
  ),
);
