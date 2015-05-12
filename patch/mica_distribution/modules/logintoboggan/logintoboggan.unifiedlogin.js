
(function ($) {

Drupal.behaviors.unifiedLogin = {

  attach: function (context) {
    // Attach behaviors to the links so that they show/hide forms appropriately.
    $('.toboggan-unified #register-link').click(function() {
      $(this).addClass('btn-primary').blur();
      $('.toboggan-unified #login-link').removeClass('btn-primary');
      $('.toboggan-unified #register-form').show();
      $('.toboggan-unified #login-form').hide();
      Drupal.settings.LoginToboggan.unifiedLoginActiveForm = 'register';
      return false;
    });
    $('.toboggan-unified #login-link').click(function() {
      $(this).addClass('btn-primary').blur();
      $('.toboggan-unified #register-link').removeClass('btn-primary');
      $('.toboggan-unified #login-form').show();
      $('.toboggan-unified #register-form').hide();
        Drupal.settings.LoginToboggan.unifiedLoginActiveForm = 'login';
      return false;
    });

    switch(Drupal.settings.LoginToboggan.unifiedLoginActiveForm) {
      case 'register':
        $('.toboggan-unified #register-link').click();
        break;
      case 'login':
      default:
        $('.toboggan-unified #login-link').click();
        break;
    }
  }
};

})(jQuery);

