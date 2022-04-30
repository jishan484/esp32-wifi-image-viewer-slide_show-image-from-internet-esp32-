
const express = require("express");
const app = express();
const request = require('request');
var fs = require('fs')
const { exec } = require('child_process');


const url = 'https://picsum.photos/240';
var uul = "https://cdn.glitch.com/eedb018f-f384-4f9f-82df-cdfeca5ba0be%2Fasas.jpg?v=1579115904898"
var next = "";

app.use(express.static("public"));

app.get("/image",(req,res)=>{
  res.sendFile(`${__dirname}/googleo.jpg`);
  
  next = "";
  download(url, 'googleo.jpg', function(){
    exec('convert googleo.jpg googlei.jpeg && xxd -i googlei.jpeg | sed "1s/.*//" | sed "s/ [a-zA-Z_]*l//"', (err, stdout, stderr) => {
     stdout=stdout.split("}");
      next +=stdout[0];
      next = next.replace(/ /gi,"");
});   
  });
})

app.get("/", (req, response) => {
  if(next == "")
  {
    response.sendFile(`${__dirname}/iimg.txt`);
  }
  else
  {
    response.send(next.toString()); 
  }
  next = "";
  download(url, 'googleo.jpg', function(){
    exec('convert googleo.jpg googlei.jpeg && xxd -i googlei.jpeg | sed "1s/.*//" | sed "s/ [a-zA-Z_]*l//"', (err, stdout, stderr) => {
     stdout=stdout.split("}");
      next +=stdout[0];
      next = next.replace(/ /gi,"");
});   
  });
}); 


var listener = app.listen(process.env.PORT, () => {
  console.log(`Your app is listening on port ${listener.address().port}`);
});

var download = function(uri, filename, callback){
  request.head(uri, function(err, res, body){
    console.log('content-type:', res.headers['content-type']);
    console.log('content-length:', res.headers['content-length']);
    request(uri).pipe(fs.createWriteStream(filename)).on('close', callback);
  });
};
