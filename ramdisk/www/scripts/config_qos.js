var	msgApplyOK = 0;
var	msgApplyFailed = 1;
var msgConfirm = 2;
var	msg;

function onInit()
{
	msg = new Array();

	msg[msgApplyOK] = '네트워크 정보가 정상적으로 변경되었습니다.';
	msg[msgApplyFailed] = '네트워크 정보 변경에 문제가 발생하였습니다.';
	msg[msgConfirm] = '네트워크 정보를 수정하고 시스템을 다시 시작하시겠습니까?';

	document.getElementById('section2_title').innerHTML='Qos 설정';
	document.getElementById('page_title').innerHTML='Qos 설정';
	document.getElementById('apply').value='적용';
	document.getElementById('body').hidden = false;
	
}

function onLoad()
{
	onInit();
	loadQOS();
}

function loadQOS()
{
	if(typeof window.ActiveXObject != 'undefined')
	{
		xmlhttp = (new ActiveXObject("Microsoft.XMLHTTP"));
	}
	else
	{
		xmlhttp = (new XMLHttpRequest());
	}
	
	var data = "/cgi-bin/qos?cmd=state";

	xmlhttp.open( "POST", data, true );
	xmlhttp.setRequestHeader("Content-Type","application/x-www-form-urlencoded;charset=euc-kr");
	xmlhttp.onreadystatechange = function()
	{
		if( (xmlhttp.readyState == 4) && (xmlhttp.status == 200) )
		{
			try
            {
            	result = xmlhttp.responseXML.documentElement.getElementsByTagName("res")[0];
            	if (result.firstChild.nodeValue == 'OK') {
            		// 파싱
            		var result2 = xmlhttp.responseXML.documentElement.getElementsByTagName("text")[0];
            		var text = result2.firstChild.nodeValue;

					if (text == "done")
					{
						alert("Please Refresh..");
						return;
					}
					var trimText = trim(text);
					var textArr = trimText.split("||");
					var ATCOMMAND = textArr[0].split(":")[0];
					var ATCOMMAND_RES = textArr[0].split(":")[1];

					var cid = ATCOMMAND_RES.split(",")[0];
					var traffic = ATCOMMAND_RES.split(",")[1];
					var uplink = ATCOMMAND_RES.split(",")[2];
					var dnlink = ATCOMMAND_RES.split(",")[3];
					
					qosArr = ATCOMMAND_RES.split(",");
					//alert(qosArr);
					//var cid_select = document.getElementById("cidSelect");
					var traffic_select = document.getElementById("trafficSelect");
					//cid_select[Number(cid)-1].selected = true;
					traffic_select[Number(traffic)].selected = true;
					
					var uplinkTF = document.getElementById("uplink");
					var dnlinkTF = document.getElementById("dnlink");
					uplinkTF.value = uplink;
					dnlinkTF.value = dnlink;
            	} else {
            		// error
            		alert("Please Refresh..");
            	}
            }
            catch(e)
            {
            }
		}
	}
	xmlhttp.send();
}


function setQOS()
{
	if(typeof window.ActiveXObject != 'undefined') {
		xmlhttp = (new ActiveXObject("Microsoft.XMLHTTP"));
	} else {
		xmlhttp = (new XMLHttpRequest());
	}
	
	var sms_select = document.getElementById("smsSelect");
	var data = "/cgi-bin/qos?cmd=set"

	//var cid_select = document.getElementById("cidSelect");
	var traffic_select = document.getElementById("trafficSelect");
	var uplinkTF = document.getElementById("uplink");
	var dnlinkTF = document.getElementById("dnlink");

	data += "&cid=2"; //cid_select[cid_select.selectedIndex].value;
	data += "&traffic=" + traffic_select.selectedIndex;
	data += "&mb_uplink=" + uplinkTF.value;
	data += "&mb_dnlink=" + dnlinkTF.value;
	/*
	data += "&gb_uplink=" + qosArr[4];
	data += "&gb_dnlink=" + qosArr[5];
	data += "&delivery_order=" + qosArr[6];
	data += "&max_sdu_size=" + qosArr[7];
	data += "&sdu_error_ratio=" + qosArr[8];
	data += "&residual_bit_error_ratio=" + qosArr[9];
	data += "&delivery_error_sdus=" + qosArr[10];
	data += "&transfer_delay=" + qosArr[11];
	data += "&traffic_handling_priority=" + qosArr[12];
	*/
	xmlhttp.open( "POST", data, true );
	xmlhttp.setRequestHeader("Content-Type","application/x-www-form-urlencoded;charset=euc-kr");
	xmlhttp.onreadystatechange = function()
	{
		if( (xmlhttp.readyState == 4) && (xmlhttp.status == 200) )
		{
			try
            {
            	result = xmlhttp.responseXML.documentElement.getElementsByTagName("res")[0];
            	if (result.firstChild.nodeValue == 'OK') {
            		alert("Qos : OK");
            	} else {
            		alert("Qos : ERROR");
            	}
            }
            catch(e)
            {

            }
		}
	}
	xmlhttp.send();
}