<!DOCTYPE html>
<html>
<head>
    <title>Arduino Web Control</title>
    <style type="text/css">
        body {
            background-color: #f0f0f2;
            margin: 0;
            padding: 0;
            font-family: "Open Sans", "Helvetica Neue", Helvetica, Arial, sans-serif;
        }

        #normal {

	   vertical-align: middle;
           width: 900px;
           height: auto;
            margin: 1em auto;
           padding: 50px;
            background-color: #fff;
             border-radius: 1em;

        }


        .normal {
	    
	/*box-shadow: inset 0 0 40px black;*/
	    font-size: 40px;
	    width: 200px;
	    height: auto;
            color: black;
            text-align: center;
            background-color: #f0f0f2;


	    margin: 0 auto;
            padding: 20px;
            border-radius: 1em;
	    line-height:-5px;
        }

        
	.temperatura {
	    float:left;
	    font-size: 40px;
	    width: 200px;
	    height: 150px;
            color: black;
            text-align: center;
            background-color: #f0f0f2;


	    margin: 1em auto;
            padding: 20px;
            border-radius: 1em;

	    line-height:150px;

        }


        a:link, a:visited {
            color: #38488f;
            text-decoration: none;
        }

        @media (max-width: 700px) {

            body {
                background-color: #fff;
            }

            div {
                width: auto;
                margin: 0 auto;
                border-radius: 0;
                padding: 1em;
            }
        }
    </style>

    <script src="http://ajax.aspnetcdn.com/ajax/jQuery/jquery-1.7.min.js"></script>

    <script language="javascript" type="text/javascript">
        var light = 0;
        var fan = 0;
        var temperature = 0.000;
        var stringIN = "";

        function toGet(charIN) {
            //alert(charIN);

            $.get("http://192.168.1.177:1212/" + charIN);

	    if(charIN == 'a'){
		light = !light;
		atualizaLight();
	    }
	    else if (charIN == 'b'){
		fan = !fan;
		atualizaFan();
	    }
		
        }

	function atualizaLight(){
			if(light == 0)
		        	document.getElementById('LI').innerHTML = "ON";
			else
		        	document.getElementById('LI').innerHTML = "OFF";
	}

	function atualizaFan(){
			if(fan == 0)
		        	document.getElementById('FA').innerHTML = "ON";
			else
		       		 document.getElementById('FA').innerHTML = "OFF";
	}
	

	function startTime(autoChar){
		$.ajax({
		  url: 'get.php',
		  success: function(data){
			//alert(data);
			var txt = data;		    
			txt = txt.split("\n");
			light = txt[0];
			fan = txt[1];
			temperature = txt[2];

			atualizaLight();
			atualizaFan();

		        document.getElementById('TE').innerHTML = temperature + 'º';

			if(autoChar == '1') setTimeout(function(){startTime('1')}, 2000)
		  }
		});
	}

    </script>
</head>

<body>
<body onload="startTime('1')">

    <div id ="normal" style="text-align:center;">
        <h1>Arduino Web Control</h1>

	<div class="temperatura" id="TE">
	</div>

	<div class="normal"  style="float:right;">
        <input type="image" onclick="toGet('a');" src="bulb.png" style="width:200px;">
        <text id="LI"></text>
	</div>

	<div class="normal" >
        <input type="image" onclick="toGet('b');" src="fan.png" style="width:200px;">
        <text id="FA"></text>
	</div>
  </marquee>
</marquee>

    </div>

</body>
</html>
