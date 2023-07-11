const app = require("express")();
var bodyParser = require("body-parser");
const cors = require("cors");
const mqtt = require("mqtt");

const PORT = 4567;
const client = mqtt.connect("mqtt://localhost:1883");

app.use(cors({ origin: "*" }));
//*************************  SERVEUR WEB  *************************** */

//BD
var idlist = [];

//GET
app.get("/idlist", (req, res) => {
  res.status(200).send(idlist);
});

//POST
app.post("/idlist", bodyParser.json(), (req, res) => {
  let data = req.body;
  data.time =
    new Date().toLocaleDateString() + " " + new Date().toLocaleTimeString();
  res.status(200).send("Data Received: " + JSON.stringify(data));
  idlist.push(data);
  client.publish(`hello/world`, JSON.stringify(data), { qos: 1 });
});

//**************** ARCHIVES  ********************* */

//BD
var name_ids = [
  { id: "2f234454-cf6d-4a0f-adf2-f4911ba9ffa6", name: "sacha" },
  { id: "8819e741-a467-4b19-8a72-57a6f5dce2db", name: "antho" },
];

//ARCHIVE
var namelist = [];

function getName(id) {
  for (let el of name_ids) {
    if(el.id === id){
      return el.name;
    }
  }
}

//GET
app.get("/namelist", (req, res) => {
  res.status(200).send(namelist);
});

//POST-ISH
client.subscribe("hello/world", 1, (error, res) => {
    if (error) {
      console.log("Subscribe to topics error", error);
      return;
    }
    console.log("Subscribe to topics res", res);
});
client.on("message",(topic, data)=>{
    update_archives(JSON.parse(data.toString()))
});

function update_archives(data) {
  data.name = getName(data.id);
  data.time = new Date().toLocaleDateString() + " " + new Date().toLocaleTimeString();
  namelist.push(data);
}

app.listen(PORT);
