
// Static Value
var _value1 = 0;
var _value2 = {
    name:"Cooldong",
    age:29,
}

var _json_value;



// Function
function set_text_status(input)
{
    document.getElementById('ON_OFF_STATUS').value = input;
    _json_value = make_json(_value2);
    document.getElementById('JSON_PARSING').value = parse_json(_json_value).name;
}

function change_text_range()
{
    document.getElementById('RANGE_TEXT').value = document.getElementById('RANGE1').value;
}

function make_json(value)
{   
    var temp = JSON.stringify(value);
    document.getElementById('JSON_STRING').value = temp;

    return temp;
}

function parse_json(value)
{
    var temp = JSON.parse(value);
    
    return temp;
}


function toggleCheckbox(element)
{
    var xhr = new XMLHttpRequest();
    if(element.checked)
    {
        // &state=1 가 전송하는 값임, true 비동기
        xhr.open("GET", "/update?output="+element.id+"&state=1", true);        
    }
    else
    {
        xhr.open("GET", "/update?output="+element.id+"&state=0", false);
    }
    document.getElementById("TOGGLE_STATUS").value = element.checked;
    xhr.send();
}

function toggleCheckbox2(element)
{
    var xhr = new XMLHttpRequest();
    if(element.checked)
    {
        // &state=1 가 전송하는 값임, true 비동기
        xhr.open("GET", "/update?output="+element.id+"&state2=1", true);        
    }
    else
    {
        xhr.open("GET", "/update?output="+element.id+"&state2=0", false);
    }
    document.getElementById("TOGGLE_STATUS2").value = element.checked;
    xhr.send();
}
