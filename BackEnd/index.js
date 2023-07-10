const app = require('express')();
const PORT = 4567
var bodyParser = require('body-parser')

var idlist = 
[
    {"id":1, "piece":"bigbang", "time":0},
    {"id":2, "piece":"bigbang", "time":0}
]

app.get('/idlist', (req, res)=>{
    res.status(200).send(idlist)
})
app.post('/idlist', bodyParser.json(), (req, res) => {
    let data = req.body;
    data.time= new Date().toLocaleDateString() + " " + new Date().toLocaleTimeString();
    res.status(200).send('Data Received: ' + JSON.stringify(data));
    idlist.push(data);
    //evoyer sur mqtt
})

//**************** ARCHIVES  ********************* */

var name_ids = 
[
    {"id":1, "name":"sacha"},
    {"id":2, "name":"antho"}
]

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

app.get('/namelist', (req, res)=>{
    res.status(200).send(namelist)
})

//sur message.on
app.post('/namelist', bodyParser.json(), (req, res) => {
    let data = req.body;
    data.name=getName(data.id);
    data.time= new Date().toLocaleDateString() + " " + new Date().toLocaleTimeString();
    res.status(200).send('Data Received: ' + JSON.stringify(data));
    namelist.push(data);
    //evoyer sur mqtt
})

app.listen(PORT)