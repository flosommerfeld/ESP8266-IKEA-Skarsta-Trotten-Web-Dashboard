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
                    <h4 class="my-0 font-weight-normal">Controls</h4>
                 </div>
                 <div class="card-body">
                    <ul class="list-unstyled mt-3 mb-4">
                       <li>Manual height adjustment</li>
                    </ul>
                    <a type="button" class="btn btn-lg btn-block btn-outline-primary" href="/motor/up"><i class="bi bi-arrow-up-square-fill"></i></a>
                    <a type="button" class="btn btn-lg btn-block btn-outline-primary" href="/motor/stop"><i class="bi bi-pause-btn-fill"></i></a>
                    <a type="button" class="btn btn-lg btn-block btn-outline-primary" href="/motor/down"><i class="bi bi-arrow-down-square-fill"></i></a>
                 </div>
              </div>
              <div class="card mb-4 box-shadow">
                 <div class="card-header">
                    <h4 class="my-0 font-weight-normal">Modes</h4>
                 </div>
                 <div class="card-body">
                    <ul class="list-unstyled mt-3 mb-4">
                       <li>Predefined modes</li>
                    </ul>
                    <div id="mode-container"><!-- This is the element into which the modes from the local storage will be loaded --></div>
                    
                    <a type="button" class="btn btn-lg btn-block btn-primary" href="/height/75">Mode 1 - 75 cm</a>
                    <a type="button" class="btn btn-lg btn-block btn-primary" href="/height/125">Mode 2 - 125 cm</a>
                    <a type="button" class="btn btn-lg btn-block btn-primary" href="/height/150">Mode 3 - 150 cm</a>
                 </div>
              </div>
           </div>
        </div>

         <script>
            const LOCALSTORAGE_ITEM_NAME = "modes"; // identifier of the saved modes
            const DEFAULT_MODES = ["75", "125", "150"]; // local storage will saves everything as strings and these values will be URI parameters so it is fine to not use integers
            const MODE_CONTAINER_ELEMENT_ID = "mode-container"; // Note: if you change this, please also change the corresponding id in HTML element

            /* 
             * Returns the saved modes from the local storage as asn array of strings.
             * Example -> ["75", "125", "150"]
             */ 
            const getModes = () => {
               let modes = localStorage.getItem(LOCALSTORAGE_ITEM_NAME);
               // check if the item exists, otherwise initialize it with the default values
               if (modes) {
                  return JSON.parse(modes);
               }
               else {
                  localStorage.setItem(LOCALSTORAGE_ITEM_NAME, JSON.stringify(DEFAULT_MODES)); 
                  return DEFAULT_MODES;
               }
            }

            /*
             * Creates a button for which represents the mode. This button is also clickable and will do a request to the server for changing the tables height.
             * Example for an created element:  <a type="button" class="btn btn-lg btn-block btn-primary" href="/height/150">Mode 3 - 150 cm</a>
             */
            const createModeElement = (id, height) => {
               let button = document.createElement("a"); // create the element
               button.type = "button";
               button.className = "btn btn-lg btn-block btn-primary";
               button.href = "/height/" + height;
               button.innerHTML = "Mode " + id + " - " + height + " cm";

               return button; // return the element which now holds all necessary info
            }

            /*
             * Adds all passed modes to the modes container so that they are being displayed.
             */
            const displayModes = (modes) => {
               let modeContainer = document.getElementById("mode-container");
               // Create and immediately append an element for each mode as a child of the container
               for (i in modes) {
                  let newMode = createModeElement(parseInt(i)+1, modes[i]); // Create the element, Note: +1 because it shall start at 1 -> "Mode 1 - 75 cm"
                  modeContainer.appendChild(newMode); // Add element to DOM
               }
            }

            // get all locally saved modes
            let foundModes = getModes();

            // display all of the locally saved modes
            displayModes(foundModes);
         </script> 

     </body>
  </html>
)=====";
