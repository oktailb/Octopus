var current;

calcHeight = function(id)
{
    var the_height = document.getElementById(id).contentWindow.document.body.scrollHeight;
    document.getElementById(id).height = the_height;
};


get_haut = function()
{
    var haut;
    
    if (document.body)
	{
	    haut = (document.body.clientHeight);
	}else{
	haut = (window.innerHeight);
    }
    return haut;
};

get_larg = function()
{
    var larg;

    if (document.body)
	{
	    larg = (document.body.clientWidth);
	}else{
	larg = (window.innerWidth);
    }
    return larg;
};

setSize = function(target)
{
    var iframeElement = document.getElementById(target);
    iframeElement.style.height = get_haut();
    alert (iframeElement.style.height);
};
    
getData = function(page)
{
    var xhr_object = null; 
    if(window.XMLHttpRequest) // Firefox 
	xhr_object = new XMLHttpRequest(); 
    else if(window.ActiveXObject) // Internet Explorer 
	xhr_object = new ActiveXObject("Microsoft.XMLHTTP"); 
    else
	{
	    alert("Votre navigateur ne supporte pas les objets XMLHTTPRequest..."); 
	    return; 
	} 
    xhr_object.open("GET", page, false); 
    xhr_object.send(null); 
    if(xhr_object.readyState == 4)
	{
	    return (xhr_object.responseText);
	}
    else
	{
	    return(getDataHere(page, type));
	}
};
    
function urlencode(str) {
    return escape(str.replace(/%/g, '%25').replace(/\+/g, '%2B')).replace(/%25/g, '%');
}

getDataHere = function(page, type)
{
    var xhr_object = null; 
    if(window.XMLHttpRequest) // Firefox 
	xhr_object = new XMLHttpRequest(); 
    else if(window.ActiveXObject) // Internet Explorer 
	xhr_object = new ActiveXObject("Microsoft.XMLHTTP"); 
    else
	{
	    alert("Votre navigateur ne supporte pas les objets XMLHTTPRequest..."); 
	    return; 
	}
    page = urlencode(urlencode(page));

    var url = "http://www.oktail.org/ddr/proxy.php?type=" + type + "&page=" + page;
    xhr_object.open("GET", url, false); 
    xhr_object.send(null); 
    if(xhr_object.readyState == 4)
	{
	    return (xhr_object.responseText);
	}
    else
	{
	    return("mauvaise redirection de domaine malgres feinte !\n" + url);
	}
};

getPage = function(page, target)
{
    var doc = document.getElementById(target);
    if (doc == null)
	{
	    doc = document.getElementsByTagName('html')[0];
	}
    var xhr_object = null; 
    if(window.XMLHttpRequest) // Firefox 
	xhr_object = new XMLHttpRequest(); 
    else if(window.ActiveXObject) // Internet Explorer 
	xhr_object = new ActiveXObject("Microsoft.XMLHTTP"); 
    else
	{
	    alert("Votre navigateur ne supporte pas les objets XMLHTTPRequest..."); 
	    return; 
	} 
    xhr_object.open("GET", page, false); 
    xhr_object.send(null); 
    if(xhr_object.readyState == 4)
	{
	    doc.innerHTML = xhr_object.responseText;
	    scripts = document.getElementById(target).getElementsByTagName("script");
	    for(var i = 0; i < scripts.length; i++)
		{
		    if (scripts[i] != false)
			eval(scripts[i].innerHTML);
		    if (scripts[i].getAttribute("src"))
			{
			    try {
				var todo = scripts[i].getAttribute("src");
				if (todo)
				    eval(getData(todo));
			    }
			    catch (e)
				{
				    //alert("ADDPAGE :: " + e);
				    var todo = getDataHere(scripts[i].getAttribute("src"), "text/javascript");
				    eval(todo);
				}
			}
		}
	}
    else
	{
	    alert("mauvaise redirection de domaine !");
	}
};

addPage = function(page, target)
{
    var doc = document.getElementById(target);
    if (doc == null)
	{
	    doc = document.getElementsByTagName('html')[0];
	}
    var xhr_object = null; 
    if(window.XMLHttpRequest) // Firefox 
	xhr_object = new XMLHttpRequest(); 
    else if(window.ActiveXObject) // Internet Explorer 
	xhr_object = new ActiveXObject("Microsoft.XMLHTTP"); 
    else 
	{
	    alert("Votre navigateur ne supporte pas les objets XMLHTTPRequest..."); 
	    return; 
	} 
	xhr_object.open("GET", page, false); 
	xhr_object.send(null);
    if(xhr_object.readyState == 4)
	{
	    doc.innerHTML += xhr_object.responseText;
	    scripts = document.getElementById(target).getElementsByTagName("script");
	    for(var i = 0; i < scripts.length; i++)
		{
		    if (scripts[i] != false)
			eval(scripts[i].innerHTML);
		    if (scripts[i].getAttribute("src"))
			{
			    try {
				var todo = scripts[i].getAttribute("src");
				if (todo)
				    eval(getData(todo));
			    }
			    catch (e)
				{
				    //alert("ADDPAGE :: " + e);
				    var todo = getDataHere(scripts[i].getAttribute("src"), "text/javascript");
				    eval(todo);
				}
			}
		}
	}
};

purgePage = function(target)
{
    var doc = document.getElementById(target);
    if (doc == null)
	{
	    doc = document.getElementsByTagName('html')[0];
	}
    doc.innerHTML = "";
};

pushPage = function(title, page, target)
{
    var tg = document.getElementById(target);
    var ti = document.getElementById('titre');
    titre.innerHTML = '<h1>' + title + '</h1>';
    current = page;
    tg.innerHTML= '<iframe id="doc_content" width="100%" height="100%" style="border:none;" src="' + page + '">oh, un brontosaure ! ou un chauve peut etre ?</iframe>';
};

getElementPos = function(element, xPosition)
{
    var position = 0;
    
    if (element)
	{
	    var elementOffsetParent = element.offsetParent;
	    if (elementOffsetParent)
		{
		    while ((elementOffsetParent = element.offsetPArent) != null)
			{
			    if (xPosition)
				position += element.offsetLeft;
			    else
				position += element.offsetTop;
			}
		}
	    else
		{
		    if (xPosition)
			position += element.offsetLeft;
		    else
			position += element.offsetTop;
		}
	}
    return position;
};

getElementPosX = function(element)
{
    return getElementPos(element, true);
};

getElementPosY = function(element)
{
    return getElementPos(element, false);
};

var x;
var y;

corig_pos = function(target)
{
  var my_x = x;
  var my_y = y;
  var width;
  var heigth;

  var my_id = document.getElementById(target);
  var haut = get_haut();
  var larg = get_larg();

  var img = my_id.getElementsByTagName('img')[0];
  var obj = my_id.getElementsByTagName('embed')[0];

  var pied = document.getElementById('pied');

  pied.innerHTML = "";
  if (img)
  {
    pied.innerHTML += "IMG -> ";
    pied.innerHTML += "Width = "+img.width;
    pied.innerHTML += " Height = "+img.height;
    width = img.width + 20;
    height = img.height + 20;
    pied.innerHTML += " src = "+img.src;
  }
  else if (obj)
  {
    pied.innerHTML += "OBJ -> ";
    pied.innerHTML += "Width = "+obj.width;
    pied.innerHTML += " Height = "+obj.height;
    width = obj.width + 20;
    height = obj.height + 20;
    pied.innerHTML += " src = "+obj.src;
  }

  my_id.style.position = 'fixed';

  if (larg < (my_x + width))
  {
     my_id.style.right = larg - my_x;
     my_id.style.left = 'auto';
  }
  else
     my_id.style.left = my_x;
  if (haut < (my_y + height))
  {
     my_id.style.bottom = haut - my_y;
     my_id.style.top = 'auto';
  }
  else
     my_id.style.top = my_y;
};

setPosFromBottomRight = function(elem)
{
    try {
    var h = get_haut();
    var l = get_larg();
    var nh = h - elem.height;
    var nl = l - elem.width;
    
    elem.style.position= "absolute";
    elem.style.top = nh;
    elem.style.left = nl;
    }
    catch (e)
	{
	    alert(e);
	}
};

setPos = function(elem, x, y, z)
{
    try {
    elem.style.position= "absolute";
    elem.style.top = y;
    elem.style.left = x;
    elem.style.zIndex = z;
    }
    catch (e)
	{
	    alert(e);
	}
};

setElemSize = function(elem, w, h)
{
    try {
	if (w == '100%')
	    {
		w = get_larg() - elem.style.left;
	    }
	if (h == '100%')
	    {
		h = get_haut() - elem.style.top;
	    }
	elem.style.width = w;
	elem.style.height = h;
    }
    catch (e)
	{
	    alert(e);
	}
};

init_clock = function() {
    anal_clock();
    setInterval('anal_clock()', 1000);
};

init_clock2 = function() {
    bin_clock();
    setInterval('bin_clock()', 1000);
};

anal_clock = function() {
    var size = 90;
    var now = new Date();
    var ctx = document.getElementById('canvas_anim').getContext('2d');
    ctx.save();
    ctx.clearRect(0, 0, size, size);
    ctx.translate(size/2, size/2);
    ctx.scale(0.4, 0.4);
    ctx.rotate(-Math.PI/2);
    ctx.strokeStyle = "white";
    ctx.fillStyle = "white";
    ctx.lineWidth = 8;
    ctx.lineCap = "round";
    
    // Marques des heures
    ctx.save();
    ctx.beginPath();
    for (i = 0; i < 12; i++) {
	ctx.rotate(Math.PI/6);
	ctx.moveTo(100, 0);
	ctx.lineTo(120, 0);
    }
    ctx.stroke();
    ctx.restore();
    
    // Marques des minutes
    ctx.save();
    ctx.lineWidth = 5;
    ctx.beginPath();
    for (i = 0; i < 60; i++) {
	if (i%5 != 0) {
	    ctx.moveTo(117, 0);
	    ctx.lineTo(120, 0);
	}
	ctx.rotate(Math.PI/30);
    }
    ctx.stroke();
    ctx.restore();
    
    var sec = now.getSeconds();
    var min = now.getMinutes();
    var hr  = now.getHours();
    var ms = now.getMilliseconds();
    //    setPosAndSize('coin1'   , 320   + ms, 240, 0     , 0     , 1);
    //    setPosAndSize('coin2'   , 320   + ms, 240, '100%', '100%', 6);

    hr = (hr >= 12) ? hr-12 : hr;
    
    ctx.fillStyle = "black";
    
    // Dessin des heures
    ctx.save();
    ctx.rotate( hr*(Math.PI/6) + (Math.PI/360)*min + (Math.PI/21600)*sec
		)
	ctx.lineWidth = 14;
    ctx.beginPath();
    ctx.moveTo(-20, 0);
    ctx.lineTo(size/2, 0);
    ctx.stroke();
    ctx.restore();
    
    // Dessin des minutes
    ctx.save();
    ctx.rotate( (Math.PI/30)*min + (Math.PI/1800)*sec );
    ctx.lineWidth = 10;
    ctx.beginPath();
    ctx.moveTo(-28, 0);
    ctx.lineTo(size, 0);
    ctx.stroke();
    ctx.restore();
    
    // Dessin des secondes
    ctx.save();
    ctx.rotate(sec * Math.PI/30);
    ctx.strokeStyle = "#D40000";
    ctx.fillStyle = "#D40000";
    ctx.lineWidth = 6;
    ctx.beginPath();
    ctx.moveTo(-30, 0);
    ctx.lineTo(size, 0);
    ctx.stroke();
    ctx.beginPath();
    ctx.arc(0, 0, 10, 0, Math.PI*2, true);
    ctx.fill();
    ctx.beginPath();
    ctx.arc(size, 0, 10, 0, Math.PI*2, true);
    ctx.stroke();
    ctx.fillStyle = "#555";
    ctx.arc(0, 0, 3, 0, Math.PI*2, true);
    ctx.fill();
    ctx.restore();
    
    ctx.beginPath();
    ctx.lineWidth = 14;
    ctx.strokeStyle = '#FFA700';
    ctx.arc(0, 0, 142, 0, Math.PI*2, true);
    ctx.stroke();
    
    ctx.restore();
};

bin_clock = function() {
    var size = 140;
    var now = new Date();
    var ctx = document.getElementById('canvas_anim').getContext('2d');
    ctx.save();
    ctx.clearRect(0, 0, size, size);
    ctx.strokeStyle = "grey";
    ctx.fillStyle = "orange";
    ctx.lineWidth = 8;
    ctx.lineCap = "round";
    
    var digit0 = 0;
    var digit1 = 0;
    var digit2 = 0;
    var digit3 = 0;
    
    var sec = now.getSeconds();
    var min = now.getMinutes();
    var hr  = now.getHours();
    var ms = now.getMilliseconds();
    //    setPosAndSize('coin1'   , 320   + ms, 240, 0     , 0     , 1);
    //    setPosAndSize('coin2'   , 320   + ms, 240, '100%', '100%', 6);

    //    hr = (hr >= 12) ? hr-12 : hr;
    ctx.fillStyle = "white";
    
    var PI = Math.PI;
    var work;
    var worku;
    var workd;
    // Dessin des heures
    ctx.save();
    ctx.lineWidth = 1;
    ctx.beginPath();
    work = hr;
    worku = work%10;
    workd = (work-worku)/10;

    digit0 = workd%2;
    ctx.beginPath();
    ctx.arc(8, 56, 8, 0, 2*PI, 0);
    if (digit0){ctx.fill();}else{ctx.stroke();}
    workd = (workd - digit0)/2
    digit1 = workd%2;
    ctx.beginPath();
    ctx.arc(8, 40, 8, 0, 2*PI, 0);
    if (digit1){ctx.fill();}else{ctx.stroke();}
    workd = (workd - digit1)/2
    digit2 = workd%2;
    ctx.beginPath();
    //    ctx.arc(8, 24, 8, 0, 2*PI, 0);
    //    if (digit2){ctx.fill();}else{ctx.stroke();}
    workd = (workd - digit2)/2
    digit3 = workd%2;
    ctx.beginPath();
    //    ctx.arc(8, 8, 8, 0, 2*PI, 0);
    //    if (digit3){ctx.fill();}else{ctx.stroke();}
    workd = (workd - digit3)/2


    digit0 = worku%2;
    ctx.beginPath();
    ctx.arc(24, 56, 8, 0, 2*PI, 0);
    if (digit0){ctx.fill();}else{ctx.stroke();}
    worku = (worku - digit0)/2
    digit1 = worku%2;
    ctx.beginPath();
    ctx.arc(24, 40, 8, 0, 2*PI, 0);
    if (digit1){ctx.fill();}else{ctx.stroke();}
    worku = (worku - digit1)/2
    digit2 = worku%2;
    ctx.beginPath();
    ctx.arc(24, 24, 8, 0, 2*PI, 0);
    if (digit2){ctx.fill();}else{ctx.stroke();}
    worku = (worku - digit2)/2
    digit3 = worku%2;
    ctx.beginPath();
    ctx.arc(24, 8, 8, 0, 2*PI, 0);
    if (digit3){ctx.fill();}else{ctx.stroke();}
    worku = (worku - digit3)/2

    ctx.restore();
    
    // Dessin des minutes
    ctx.save();
    ctx.lineWidth = 1;
    ctx.beginPath();
    work = min;
    worku = work%10;
    workd = (work-worku)/10;

    digit0 = workd%2;
    ctx.beginPath();
    ctx.arc(40, 56, 8, 0, 2*PI, 0);
    if (digit0){ctx.fill();}else{ctx.stroke();}
    workd = (workd - digit0)/2
    digit1 = workd%2;
    ctx.beginPath();
    ctx.arc(40, 40, 8, 0, 2*PI, 0);
    if (digit1){ctx.fill();}else{ctx.stroke();}
    workd = (workd - digit1)/2
    digit2 = workd%2;
    ctx.beginPath();
    ctx.arc(40, 24, 8, 0, 2*PI, 0);
    if (digit2){ctx.fill();}else{ctx.stroke();}
    workd = (workd - digit2)/2
    digit3 = workd%2;
    ctx.beginPath();
    //    ctx.arc(40, 8, 8, 0, 2*PI, 0);
    //    if (digit3){ctx.fill();}else{ctx.stroke();}
    workd = (workd - digit3)/2


    digit0 = worku%2;
    ctx.beginPath();
    ctx.arc(56, 56, 8, 0, 2*PI, 0);
    if (digit0){ctx.fill();}else{ctx.stroke();}
    worku = (worku - digit0)/2
    digit1 = worku%2;
    ctx.beginPath();
    ctx.arc(56, 40, 8, 0, 2*PI, 0);
    if (digit1){ctx.fill();}else{ctx.stroke();}
    worku = (worku - digit1)/2
    digit2 = worku%2;
    ctx.beginPath();
    ctx.arc(56, 24, 8, 0, 2*PI, 0);
    if (digit2){ctx.fill();}else{ctx.stroke();}
    worku = (worku - digit2)/2
    digit3 = worku%2;
    ctx.beginPath();
    ctx.arc(56, 8, 8, 0, 2*PI, 0);
    if (digit3){ctx.fill();}else{ctx.stroke();}
    worku = (worku - digit3)/2

    ctx.restore();
    
    // Dessin des secondes
    ctx.save();
    ctx.lineWidth = 1;
    work = sec;
    worku = work%10;
    workd = (work-worku)/10;

    digit0 = workd%2;
    ctx.beginPath();
    ctx.arc(72, 56, 8, 0, 2*PI, 0);
    if (digit0){ctx.fill();}else{ctx.stroke();}
    workd = (workd - digit0)/2
    digit1 = workd%2;
    ctx.beginPath();
    ctx.arc(72, 40, 8, 0, 2*PI, 0);
    if (digit1){ctx.fill();}else{ctx.stroke();}
    workd = (workd - digit1)/2
    digit2 = workd%2;
    ctx.beginPath();
    ctx.arc(72, 24, 8, 0, 2*PI, 0);
    if (digit2){ctx.fill();}else{ctx.stroke();}
    workd = (workd - digit2)/2
    digit3 = workd%2;
    ctx.beginPath();
    //    ctx.arc(72, 8, 8, 0, 2*PI, 0);
    //    if (digit3){ctx.fill();}else{ctx.stroke();}
    workd = (workd - digit3)/2

    digit0 = worku%2;
    ctx.beginPath();
    ctx.arc(88, 56, 8, 0, 2*PI, 0);
    if (digit0){ctx.fill();}else{ctx.stroke();}
    worku = (worku - digit0)/2
    digit1 = worku%2;
    ctx.beginPath();
    ctx.arc(88, 40, 8, 0, 2*PI, 0);
    if (digit1){ctx.fill();}else{ctx.stroke();}
    worku = (worku - digit1)/2
    digit2 = worku%2;
    ctx.beginPath();
    ctx.arc(88, 24, 8, 0, 2*PI, 0);
    if (digit2){ctx.fill();}else{ctx.stroke();}
    worku = (worku - digit2)/2
    digit3 = worku%2;
    ctx.beginPath();
    ctx.arc(88, 8, 8, 0, 2*PI, 0);
    if (digit3){ctx.fill();}else{ctx.stroke();}
    worku = (worku - digit3)/2
    ctx.restore();
    }

my_imc = function() {
    var sizew = 170;
    var sizeh = get_haut();
    var now = new Date();
    var ctx = document.getElementById('canvas_courbes').getContext('2d');

    ctx.save();
    ctx.clearRect(0, 0, sizew, sizeh);
    ctx.fillStyle = "white";
    ctx.strokeStyle = "orange";
    ctx.lineWidth = 4;
    ctx.beginPath();
    ctx.moveTo(0,0);
    ctx.lineTo(170, get_haut()/8);
    ctx.fill();
    ctx.stroke();
    ctx.restore();
};



current = 'http://www.oktail.org/intro.php';

setPosAndSize = function(elem, w, h, x, y, z)
{
    try {
	elem = document.getElementById(elem);
	if (x == '100%')
	    x = get_larg() - w;
	if (y == '100%')
	    y = get_haut() - h;
	if (w == '100%')
	    w = get_larg() - x;
	if (h == '100%')
	    h = get_haut() - y;
	if (x < 0)
	    x = get_larg() + x;
	if (y < 0)
	    y = get_haut() + y;
	if (w < 0)
	    w = get_larg() - x + w;
	if (h < 0)
	    h = get_haut() - y + h;
	elem.style.position= "absolute";
	elem.style.top = y;
	elem.style.left = x;
	elem.style.zIndex = z;
	elem.style.width = w;
	elem.style.height = h;
    }
    catch (e)
	{
	    alert(e);
	}
};

