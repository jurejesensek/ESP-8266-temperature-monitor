<?php
	include_once("glava.php");
	
	$od = isset($_GET['od']) ? $_GET['od'] : '';
	$do = isset($_GET['do']) ? $_GET['do'] : '';
	$pogoj = "";
	if(!empty($od)){
		$pogoj = "created_at > '" . date('Y-m-d H:i:s', strtotime($od) - 7200) . "' ";
	}
	if(!empty($do)){
		if(!empty($pogoj)){
			$pogoj .= "AND ";
		}
		$pogoj .= "created_at < '" . date('Y-m-d H:i:s', strtotime($do) - 7200) . "' ";
	}
	
	$sql = "SELECT * FROM results ";
	if(!empty($pogoj)){
		$sql .= "WHERE " . $pogoj;
	}
	$sql .= "ORDER BY sender, sensor, id DESC";
	//die($sql);
	$ret = $db->query($sql); ?> <!DOCTYPE HTML> <html> <head>
	<title>Zgodovina temperatur</title>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css">
	<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
	<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js"></script>
	<script>
		window.onload = function () {
		CanvasJS.addCultureInfo("sl",
			{
				shortMonths: ["Jan", "Feb", "Mar", "Apr", "Maj", "Jun", "Jul", "Avg", "Sep", "Okt", "Nov", "Dec"]
			});

		var chart = new CanvasJS.Chart("chartContainer", {
			animationEnabled: true,
			culture: "sl",
			title:{
				text: "Zajete temperature"
			},
			axisX: {
				valueFormatString: "HH:mm:ss, DD MMM"
			},
			axisY: {
				title: "Temperature (v °C)",
				includeZero: false,
				suffix: " °C"
			},
			legend:{
				cursor: "pointer",
				fontSize: 16,
				itemclick: toggleDataSeries
			},
			toolTip:{
				shared: true
			},
			data: [
			<?php
				$previousNaprava = 0;
				$previousSenzor = 0;
				$zacetek = false;
				while($row = $ret->fetchArray(SQLITE3_ASSOC) ) {
					$temperature = floatval($row['temperature']);
					if($temperature == 0) continue;
					$naprava = $row['sender'];
					$senzor = $row['sensor'];
					if($previousNaprava != $naprava || $previousSenzor != $senzor){
						$previousNaprava = $naprava;
						$previousSenzor = $senzor;
						if($zacetek){
							echo "]},\n";
						}
						$zacetek = true;
						echo '{ name: "Naprava ' . $naprava . (!empty($senzor) ? ', senzor ' . $senzor : '' ) . '",
							type: "spline",
							yValueFormatString: "#0.## °C",
							showInLegend: true,
							dataPoints: [';
						
					}
					
					echo '{ x: new Date(' . strtotime($row['created_at']) * 1000 . '), y: ' . $temperature . ' },';
					
				}
				if($zacetek){
					echo ']}';
				}
			?>
				
			
			]
		});
		chart.render();
		function toggleDataSeries(e){
			if (typeof(e.dataSeries.visible) === "undefined" || e.dataSeries.visible) {
				e.dataSeries.visible = false;
			}
			else{
				e.dataSeries.visible = true;
			}
			chart.render();
		}
		}
	</script> </head> <body>
	<div class="row">
		<div class="col-md-4"></div>
		<div class="col-md-5">
			<form class="form-inline" action="graph.php" method="GET">
				<div class="form-group">
					<label for="od">Od:</label>
					<input type="datetime-local" class="form-control" id="od" 
placeholder="Od" name="od" value="<?php echo $od; ?>">
				</div>
				<div class="form-group">
					<label for="do">Do:</label>
					<input type="datetime-local" class="form-control" id="do" 
placeholder="Do" name="do" value="<?php echo $do; ?>">
				</div>
				<button type="submit" class="btn btn-default">Uveljavi</button>
			</form>
		</div>
	</div><br>
	<div id="chartContainer" style="height: 370px; max-width: 920px; margin: 0px auto;"></div>
	<script src="https://canvasjs.com/assets/script/canvasjs.min.js"></script> </body> </html> <?php
	include_once("noga.php");
?>
