// Support functions for the HTML interface
// $Id: ui.js 1325 2006-04-10 11:33:13Z julienq $

// Ask for confirmation before resetting
function check_reset()
{
  var check = window.confirm("Do you really want to reset everything?");
  return check;
}

// Show an item and hide self, while showing the previously hidden "show"
// button
function hide(node, id)
{
  node.style.display = "none";
  document.getElementById(id).style.display = "none";
  document.getElementById("_show_" + id).style.display = "block";
}

// Same as above except the other way around
function show(node, id)
{
  node.style.display = "none";
  document.getElementById(id).style.display = "table";
  document.getElementById("_hide_" + id).style.display = "block";
}
