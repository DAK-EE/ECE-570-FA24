<!DOCTYPE html>
<html>
<style type="text/css">
	#chart-container {
        width: 640px;
        height: auto;
	}
</style>
<style>
table, th, td{
        border: 1px solid black;
        border-collapse: collapse;
}
</style>
<body>
<?php
	require __DIR__ . '/require/auth.php';
?>
<table>
	<tr>
		<th>Entry ID</th>
		<th>Data</th>
		<th>Timestamp</th>
	</tr>
	<?php
		$result = $conn->query("SELECT id, data, timestamp from mqtt_esp");
		while($row=$result->fetch_row()){
			printf("<tr><td>%s</td><td>%s</td><td>%s</td>", $row[0], $row[1], $row[2]);
		}
	?>
</table>
<?php
	$sumTemp = 0;
	$sumHum = 0;
	$count = 0;
	$result = $conn->query("SELECT data from mqtt_esp");
	while($row=$result->fetch_row()){
		$sumData = $sumData + $row[0];
	        $count = $count + 1;
	}
	printf("The average value for the data is %.02f<br />", $sumData/$count);
?>

<div id="chart-container">
	<canvas id="espCanvas"></canvas>
</div>
	<script type="text/javascript" src="https://ajax.googleapis.com/ajax/libs/jquery/3.6.0/jquery.min.js"></script>
	<script type="text/javascript" src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/3.7.1/chart.min.js"></script>
	<script type="text/javascript" src="Chartjs/espdata/espapp.js"></script>
</body>
</html>
