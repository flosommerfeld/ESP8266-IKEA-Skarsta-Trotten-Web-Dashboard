const char MAIN_page[] PROGMEM = R"=====(
  <!doctype html>
  <html lang="en">
     <head>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
        <meta name="description" content="">
        <meta name="author" content="">
        <link rel="icon" href="/docs/4.0/assets/img/favicons/favicon.ico">
        <title>Ikea Skarsta/Trotten Dashboard</title>
        <link rel="stylesheet" href="https://getbootstrap.com/docs/4.0/dist/css/bootstrap.min.css">
        <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.5.0/font/bootstrap-icons.css">
        <style>
           html {
           font-size: 14px;
           }
           @media (min-width: 768px) {
           html {
           font-size: 16px;
           }
           }
           .container {
           max-width: 960px;
           }
           .pricing-header {
           max-width: 700px;
           }
           .card-deck .card {
           min-width: 220px;
           }
           .border-top { border-top: 1px solid #e5e5e5; }
           .border-bottom { border-bottom: 1px solid #e5e5e5; }
           .box-shadow { box-shadow: 0 .25rem .75rem rgba(0, 0, 0, .05); }
        </style>
     </head>
     <body>
        <div class="d-flex flex-column flex-md-row align-items-center p-3 px-md-4 mb-3 bg-white border-bottom box-shadow">
           <h5 class="my-0 mr-md-auto font-weight-normal">Ikea Skarsta/Trotten</h5>
        </div>
        <div class="pricing-header px-3 py-3 pt-md-5 pb-md-4 mx-auto text-center">
           <h1 class="display-4">Dashboard</h1>
        </div>
        <div class="container">
           <div class="card-deck mb-3 text-center">
              <div class="card mb-4 box-shadow">
                 <div class="card-header">
                    <h4 class="my-0 font-weight-normal">Kontrolle</h4>
                 </div>
                 <div class="card-body">
                    <ul class="list-unstyled mt-3 mb-4">
                       <li>Manuelles Steuern des Motors</li>
                    </ul>
                    <a type="button" class="btn btn-lg btn-block btn-outline-primary" href="/motor/up"><i class="bi bi-arrow-up-square-fill"></i></a>
                    <a type="button" class="btn btn-lg btn-block btn-outline-primary" href="/motor/stop"><i class="bi bi-pause-btn-fill"></i></a>
                    <a type="button" class="btn btn-lg btn-block btn-outline-primary" href="/motor/down"><i class="bi bi-arrow-down-square-fill"></i></a>
                 </div>
              </div>
              <div class="card mb-4 box-shadow">
                 <div class="card-header">
                    <h4 class="my-0 font-weight-normal">Modi</h4>
                 </div>
                 <div class="card-body">
                    <ul class="list-unstyled mt-3 mb-4">
                       <li>Höhenverstellung auf voreingestellte Modi</li>
                    </ul>
                    <a type="button" class="btn btn-lg btn-block btn-primary" href="/height/75">Modus 1 - 75 cm</a>
                    <a type="button" class="btn btn-lg btn-block btn-primary" href="/height/125">Modus 2 - 125 cm</a>
                    <a type="button" class="btn btn-lg btn-block btn-primary" href="/height/150">Modus 3 - 150 cm</a>
                 </div>
              </div>
           </div>
        </div>
     </body>
  </html>
)=====";
