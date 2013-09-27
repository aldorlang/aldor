jQuery(function ($) {

   function load (data, href) {
      history.pushState(null, href, href);
      data = $(data);

      $('#menubar').replaceWith($('#menubar', data));
      $('#content').replaceWith($('#content', data));
      $('#updated').replaceWith($('#updated', data));
      $('title').replaceWith($('title', data));

      install();
   }

   function install () {
      $('#menubar a:not(#actmenu)').click(function (event) {
         var a = $(event.target);
         var href = a.attr('href');

         if (typeof href == "undefined") {
            return;
         }

         event.preventDefault();

         $.get(href + '.frag')
            .done(function (data) {
               load(data, href);
            })
            .fail(function () {
               $.get(href + '/index.frag')
                  .done(function (data) {
                     load(data, href);
                  })
                  .fail(function () {
                     window.location = href;
                  });
            });
      });
   }

   install();
});
