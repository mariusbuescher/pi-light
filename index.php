<!doctype html>
<html>
	<head>
		<title>socket for raspi light toggle</title>
<?php
$socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);

$action = isset($_GET['action']) ? $_GET['action'] : null;

$val = 't';
$buf = "";

if ($action === '1') {
	$val = '1';
} else if ($action === '0') {
	$val = '0';
} else if ($action === 't') {
	$val = 't';
} else {
	$val = 's';
}

socket_connect($socket, '127.0.0.1', 8888);
socket_write($socket, $val, 1);
socket_recv($socket, $buf, 1, MSG_WAITALL);

socket_close($socket);
?>
	</head>
	<body>
		<h1>Light is <?= $buf === '1' ? 'on' : 'off'; ?>.</h1>
		<a href="index.php">Refresh</a><br>
		<a href="index.php?action=0">Off</a><br>
		<a href="index.php?action=1">On</a><br>
		<a href="index.php?action=t">Toggle</a>
	</body>
</html>

