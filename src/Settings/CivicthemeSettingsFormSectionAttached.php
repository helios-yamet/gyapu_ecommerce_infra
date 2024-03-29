<?php

namespace Drupal\civictheme\Settings;

use Drupal\Core\Form\FormStateInterface;

/**
 * CivicTheme settings section to attach global elements.
 */
class CivicthemeSettingsFormSectionAttached extends CivicthemeSettingsFormSectionBase {

  /**
   * {@inheritdoc}
   */
  public function form(&$form, FormStateInterface &$form_state) {
    $form['#attached']['library'][] = 'civictheme/theme-settings';
  }

  /**
   * {@inheritdoc}
   */
  public function weight() {
    return 0;
  }

}
