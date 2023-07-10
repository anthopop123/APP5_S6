const app = require('express')();
const PORT = 4567
var bodyParser = require('body-parser')

const mqtt =require("mqtt");
const client = mqtt.connect("mqtt://localhost:1883");
const cors = require('cors');

app.use(cors({origin:"*"}))
//*************************  SERVEUR WEB  *************************** */

//BD
var idlist = 
[
    {"id":1, "piece":"bigbang", "time":0},
    {"id":2, "piece":"bigbang", "time":0}
]

//GET
app.get('/idlist', (req, res)=>{
    res.status(200).send(idlist)
})

//POST
app.post('/idlist', bodyParser.json(), (req, res) => {
    let data = req.body;
    data.time= new Date().toLocaleDateString() + " " + new Date().toLocaleTimeString();
    res.status(200).send('Data Received: ' + JSON.stringify(data));
    idlist.push(data);
    
    client.publish(`hello/world`, JSON.stringify(data), { qos: 1 });
})

//**************** ARCHIVES  ********************* */

//BD
var name_ids = 
[
    {"id":1, "name":"sacha"},
    {"id":2, "name":"antho"}
]

//ARCHIVE
var namelist = 
[
    {"id":1, "piece":"bigbang", "time":0},
    {"id":2, "piece":"bigbang", "time":0}
]

function getName(id){
    for (var i = 0; i < name_ids.length; i++){
        if (obj[i].name == id){
          return obj[i].name
        }
    }
}

//GET
app.get('/namelist', (req, res)=>{
    res.status(200).send(namelist)
})


//POST-ISH
client.subscribe("hello/world", 1, (error, res) => {
    if (error) {
      console.log("Subscribe to topics error", error);
      return;
    }
    console.log("Subscribe to topics res", res);
});
client.on("message",(topic, data)=>{
    update_archives(data)
});

function update_archives(data){
    data.name=getName(data.id);
    data.time= new Date().toLocaleDateString() + " " + new Date().toLocaleTimeString();
    namelist.push(data);
}

app.listen(PORT)