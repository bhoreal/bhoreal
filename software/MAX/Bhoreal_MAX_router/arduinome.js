inlets = 1;
outlets = 5;

var myval=0;

if (jsarguments.length>1)
    myval = jsarguments[1];

function bang()
{
   //outlet(0,"myvalue","is",myval);
}

function msg_int(v)
{
    //post("received int " + v + "\n");
    myval = v;
    bang();
}

function msg_float(v)
{
    //post("received float " + v + "\n");
    myval = v;
    bang();
}

function list(highIn, lowIn)
{
    //var a = arrayfromargs(arguments);
    //post("received list " + a + "\n");
    xpos = 0;
    ypos = 0;
    state = 0;
    //post("received list " + highIn + " " + lowIn + "\n");
    // TRATAMIENTO DE LAS ENTRADAS
     if( highIn < 2) {
            xpos = lowIn >> 4; 
            ypos = lowIn & 0x0f;
            state = highIn;
            //post(xpos + " " + ypos + " " + state + "\n");
            outlet(0, xpos);
            outlet(1, ypos);
            outlet(2, state);
            outlet(3, xpos, ypos, state);
      } 
	  else{
		   outlet(4, lowIn); 
	  }
		
     bang();
}

//function fnInput(x,y){
//{
//}


function anything()
{
    var a = arrayfromargs(messagename, arguments);
    //post("received message " + a + "\n");
    myval = a;
    bang();
}