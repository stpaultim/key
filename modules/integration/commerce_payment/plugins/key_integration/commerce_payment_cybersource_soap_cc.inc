<?php

/**
 * @file
 * Plugin definition for the CyberSource SOAP API - CC payment method.
 */

$plugin = array(
  'title' => t('CyberSource SOAP API - CC'),
  'description' => t('Integration with the CyberSource SOAP API - CC payment method.'),
  'type' => 'commerce_payment',
  'settings' => array(
    'method_id' => 'cybersource_soap_cc',
    'key_fields' => array('transaction_key'),
  ),
);
