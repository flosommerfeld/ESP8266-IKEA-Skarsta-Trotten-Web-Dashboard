const char MAIN_page[] PROGMEM = R"=====(
<!doctype html>
<html lang="en">
   <head>
      <meta charset="utf-8">
      <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
      <meta name="description" content="">
      <meta name="author" content="">
      <link rel="icon" href="https://www.ikea.com/favicon.ico">
      <title>IKEA Skarsta/Trotten Dashboard</title>
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

         #mode-container {
          margin-bottom: 10px;
         }

         .mode-button {
            color: white !important;
         }

         .motor-button {
            color: #007bff !important;
         }
         .motor-button:hover, .mode-trash-button:hover {
            color: white !important;
         }
      </style>
   </head>
   <body>
      <div class="d-flex flex-column flex-md-row align-items-center p-3 px-md-4 mb-3 bg-white border-bottom box-shadow">
         <h5 class="my-0 mr-md-auto font-weight-normal">IKEA Skarsta/Trotten</h5>
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
                  <a type="button" class="btn btn-lg btn-block btn-outline-primary motor-button" onclick="fetch('/motor/up').then().catch(console.error('Error while communicating with the table'))"><i class="bi bi-arrow-up-square-fill"></i></a>
                  <a type="button" class="btn btn-lg btn-block btn-outline-primary motor-button" onclick="fetch('/motor/stop').then().catch(console.error('Error while communicating with the table'))"><i class="bi bi-pause-btn-fill"></i></a>
                  <a type="button" class="btn btn-lg btn-block btn-outline-primary motor-button" onclick="fetch('/motor/down').then().catch(console.error('Error while communicating with the table'))"><i class="bi bi-arrow-down-square-fill"></i></a>
               </div>
            </div>
            <div class="card mb-4 box-shadow">
               <div class="card-header">
                  <h4 class="my-0 font-weight-normal">Modes</h4>
               </div>
               <div class="card-body">
                  <ul class="list-unstyled mt-3 mb-4">
                     <li>Adjustment via modes</li>
                  </ul>
                  <div id="mode-container" class=""><!-- This is the element into which the modes from the local storage will be loaded --></div>

                  <div class="input-group mb-3">
                      <input id="input" type="number" class="form-control" placeholder="Height in cm (min: 70, max: 120)" aria-label="" min="70" max="120">
                      <div class="input-group-append">
                          <button onclick="addMode(getElementById('input').value)" class="btn btn-outline-secondary" type="button"><i class="bi bi-plus-square-fill"></i></button>
                      </div>
                  </div>
                    
               </div>
            </div>
            
         </div>
      </div>
      <div class="container">
       <div class="card mb-4 box-shadow h-100">
          <div class="card-header">
             <h4 class="my-0 font-weight-normal">Height</h4>
          </div>
          <div class="card-body">
             <div class="progress">
                <div id="height-value" class="progress-bar" role="progressbar" style="width: 0%;"></div>
              </div>
          </div>
       </div>
      </div>

       <script>
          const LOCALSTORAGE_ITEM_NAME = "modes"; // identifier of the saved modes
          const DEFAULT_MODES = ["70", "95", "120"]; // local storage will saves everything as strings and these values will be URI parameters so it is fine to not use integers
          const MODE_CONTAINER_ELEMENT_ID = "mode-container"; // Note: if you change this, please also change the corresponding id in HTML element
          const HEIGHT_VALUE_ELEMENT_ID = "height-value"; // Note: if you change this, please also change the corresponding id in HTML element
          const MIN_MODE_HEIGHT = 70;
          const MAX_MODE_HEIGHT = 120;
          

          /*
           * Takes care of correctly setting the table height in the progess bar.
           * The height will be shown in percentage -> 100% = max height
           */ 
          const setHeightInProgessBar = (height) => {
             let progessBar = document.getElementById(HEIGHT_VALUE_ELEMENT_ID);
             progessBar.innerHTML = height + " cm"; // add text to the progress bar
             progessBar.style.width = ((height / MAX_MODE_HEIGHT) * 100) + "%"; // set progress by calculating the percentage of the current height
          }


          /*
           *  Returns the height of the table, measured with the ultrasonic sensor, by performing a XMLHttp request
           */
          const getHeight = () => {
             let request = new XMLHttpRequest();

             // is fired when readyState property changes
             request.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) { // readyState 4 = DONE, see https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest/readyState
                   setHeightInProgessBar(this.responseText);
                }
             };
             request.open("GET", "height", true);
             request.send();
          }


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
           * Removes the given mode from the local storage and performs a page
           * refresh so that the deleted mode won't be shown
           */
          const deleteMode = (id) => {
              let currentModes = getModes();
              // remove one entry at index 'id' 
              currentModes.splice(id, 1);
              // save to localstorage
              localStorage.setItem(LOCALSTORAGE_ITEM_NAME, JSON.stringify(currentModes));
              // reload the page
              location.reload(true);
          }


          /*
           * Onclick handler for the add mode button. Adds a new mode to the
           * locally saved mode if it is withing the range.
           */
          const addMode = (height) => {
              let currentModes = getModes();
              // no need to add a new mode if the height already exists in another mode 
              if(currentModes.indexOf(height.toString()) != -1) {
                  return;
              }

              // mode will only be saved if it is withing range
              if (height >= MIN_MODE_HEIGHT && height <= MAX_MODE_HEIGHT){
                   // append the new height
                  currentModes.push(height.toString());
                  
                  // sort the array before saving it
                  currentModes.sort(function(left, right) {
                      left = parseInt(left); // convert string to int in order to compare
                      right = parseInt(right);
                      return left - right;
                  });

                   // save the new and sorted mode array
                  localStorage.setItem(LOCALSTORAGE_ITEM_NAME, JSON.stringify(currentModes));
              }

              // reload the page to see the new mode
              location.reload(true);
          }


          /*
           * Creates a button for which represents the mode. This button is also clickable and will do a request to the server for changing the tables height.
           * Example for an created element:  <a type="button" class="btn btn-lg btn-block btn-primary">Mode 3 - 150 cm</a>
           * The onclick event of a button does a HTTP request to communicate with the table, so the height can be changed.
           */
          const createModeElement = (id, height) => {
             // create a button group which shall consist of the mode button and a delete mode button next to it
             let buttonGroup = document.createElement("div");     
             buttonGroup.className = "btn-group btn-block";
             buttonGroup.role = "group";

             // create the mode button 
             let button = document.createElement("a");
             button.type = "button";
             button.className = "btn btn-primary col-sm-10 mode-button";
             button.onclick = function() { fetch("/height/" + height).then().catch(console.error("Error while communicating with the table")); };
             button.innerHTML = "Mode " + (id + 1) + " - " + height + " cm"; // Start counting at 1 -> "Mode 1 - 75 cm" 

              // create the delete mode button 
             let deleteButton = document.createElement("a");
             deleteButton.type = "button";
             deleteButton.className = "btn btn-outline-primary col-sm-2 mode-trash-button";
             deleteButton.onclick = function() { deleteMode(id); }; // handler for deleting this mode
             deleteButton.innerHTML = "<i class='bi bi-trash'></i>"; // "trash" icon

             buttonGroup.appendChild(button);
             buttonGroup.appendChild(deleteButton);
             return buttonGroup; // return the element which now holds two buttons and all necessary info
          }


          /*
           * Adds all passed modes to the modes container so that they are being displayed.
           */
          const displayModes = (modes) => {
             let modeContainer = document.getElementById(MODE_CONTAINER_ELEMENT_ID);
             // Create and immediately append an element for each mode as a child of the container
             for (i in modes) {
                let newMode = createModeElement(parseInt(i), modes[i]); // Create the element
                modeContainer.appendChild(newMode); // Add element to DOM
             }
          }


          // get all locally saved modes
          let foundModes = getModes();

          // display all of the locally saved modes
          displayModes(foundModes);

          // request the current table height every 2000 ms
          setInterval(function() {
             getHeight(); // AJAX call
          }, 2000); 
       </script> 

   </body>
</html>
)=====";
