// Form functions

// Check that we really want to reset
function check_reset()
{
  var check = window.confirm("Do you really want to reset everything?");
  return check;
}

// Automatically make a modified field to be marked as OK
function update_textarea(node)
{
  node.className = "ok";
  document.getElementById("okstatus" + node.name).checked = "true";
  document.getElementById("checkstatus" + node.name).removeAttribute("checked");
  document.getElementById("todostatus" + node.name).removeAttribute("checked");
}
