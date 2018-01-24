<?php
	switch ($lang) {
		case "RU":
			$pageTitle = "Проекты";
			break;
		default:
			$pageTitle = "Projects";
	}

	require_once ("part1.html");

	$file = "xml/projects.xml";
	$bgcolor = $bgcolor1;

	function startElement($parser, $name, $attrs) {
		global $bgcolor;
		global $projectName;
		global $lang;
		global $bgcolor1, $bgcolor2;

		switch ($name) {
			case "PROJECTS":
				$desc = "DESC" . $lang;
				if ($attrs[$desc] != "")
					print "<p>$attrs[desc]</p>";
				print "<table width=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n";
				break;
			case "PROJECT":
				$name = "NAME" . $lang;
				$desc = "DESC" . $lang;
				print "\t<tr bgcolor=\"$bgcolor\">\n";

				print "\t\t<td><a href=\"project.php?file=$attrs[URL]&lang=$lang\">$attrs[$name]</a></td>\n";
				print "\t\t<td>$attrs[$desc]</td>\n";
				print "\t</div></tr>\n";

				if ($bgcolor == $bgcolor1)
					$bgcolor = $bgcolor2;
				else
					$bgcolor = $bgcolor1;
				break;
		}
	}

	function endElement($parser, $name) {
		switch ($name)
		{
			case "PROJECTS":
				print "</table>\n";
				break;
		}
	}

	$xml_parser = xml_parser_create();
	xml_set_element_handler($xml_parser, "startElement", "endElement");
	if (!($fp = fopen($file, "r"))) {
		die("could not open XML input");
	}

	while ($data = fread($fp, 4096)) {
		if (!xml_parse($xml_parser, $data, feof($fp))) {
			die(sprintf("XML error: %s at line %d",
			            xml_error_string(xml_get_error_code($xml_parser)),
			            xml_get_current_line_number($xml_parser)));
		}
	}
	xml_parser_free($xml_parser);

	require_once ("part2.html");
?>
