jQuery(function ($) {
   const COOKIE = 'aldor-code';
   var saved;

   function load () {
      try {
         saved = $.parseJSON($.cookie(COOKIE));
      } catch (e) {
         saved = {};
      }
   }

   function store () {
      $.cookie(COOKIE, JSON.stringify(saved));
   }


   function add_snippet (name, code) {
      var li = $('<li style="cursor: pointer"/>');
      $('#userdef').append(
         li.append(
            $('<a/>').text(name).click(function (event) {
               $('#code').val(code);
            })
         ).append(
            $('<img src="delete.png" style="height: 1em; width: 1em"/>').click(function (event) {
               li.remove();
               delete saved[name];
               store();
            })
         )
      );
   }

   load();
   $.each(saved, add_snippet);
   
   var spinner = $('<p class="spinner"><img src="loading.gif"/></p>');

   $('#run').click(function (event) {
      var response = $('#response');
      response.addClass("spinner");
      response.html(spinner);

      $('#run').attr("disabled", "disabled");

      $.post('http://xinutec.org:9090/', {
         code: $('#code').val(),
         line: $('#line').val()
      }, function (data) {
         response.removeClass("spinner");
         response.html(data);
         $('#run').removeAttr("disabled");
      });
   });

   $('#form').submit(function (event) {
      event.preventDefault();
      if (!$('#run').attr('disabled')) {
         $('#run').click();
      }
   });

   $('#save').click(function (event) {
      var defaultName = "Snippet #" + (Object.keys(saved).length + 1);
      var name = prompt("Enter a name for your snippet", defaultName);
      if (name == null) {
         return;
      }

      var code = $('#code').val();
      saved[name] = code;
      add_snippet(name, code);
      store();
   });

   function example (name) {
      $.get('examples/' + name + '.as', function (data) {
         $('#code').val(data);
      });
   }

   $('#examples a').click(function (event) {
      example($(event.target).text());
   });

   // append snippets menu to the main menu
   var snippets = $('#snippets');
   $('#sidebar').append(snippets.children());
   snippets.remove();

   // load introductory example
   example("intro");

   $('#line').focus();
})
