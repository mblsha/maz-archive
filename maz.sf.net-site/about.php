<?php
	switch ($lang) {
		case "RU":
			$pageTitle = "О нас";
			$members   = "Почетные мемберы:";
			$skills    = "Способности:";
			break;
		default:
			$pageTitle = "About us";
			$members   = "Members:";
			$skills    = "Skills:";
	}
	require_once ("part1.html");

	$file = "xml/about.xml";
	$bgcolor = $bgcolor1;

	$memName  = "Mr. Anonymous";
	$memPhoto = "images/logo.png";
	$memEmail = "???@nowhere.com";

	$memAlign = 0;

	function printMember() {
		global $memName, $memPhoto, $memEmail;

		print "\t\t<td valign=\"TOP\" width=\"60\">\n";
		print "\t\t\t<a href=\"mailto:$memEmail\">\n";
		print "\t\t\t\t<img src=\"$memPhoto\" alt=\"Photo\" />\n";
		print "\t\t\t</a>\n";
		print "\t\t</td>\n";
	}

	function startElement($parser, $name, $attrs) {
		global $lang;
		global $memName, $memPhoto, $memEmail;
		global $members, $skills, $bgcolor, $memAlign;
		global $bgcolor1, $bgcolor2, $bgcolor3;

		switch ($name)
		{
			case "P":
				$text = "TEXT" . $lang;
				print "<p>$attrs[$text]</p>\n\n";
				break;
			case "MEMBERS":
				print "<big><b>$members</b></big>\n";
				break;
			case "MEMBER":
				$name = "NAME" . $lang;
				$desc = "DESC" . $lang;
				$memName = $attrs[$name];
				$memPhoto = $attrs[PHOTO];
				$memEmail = $attrs[EMAIL];

				print "<table width=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n";
				print "\t<tr bgcolor=\"$bgcolor\">\n";

				if ($memAlign == 0)
					printMember ();

				print "\t\t<td>\n";
				print "\t\t\t<p><em>$attrs[$name]</em></p>\n";
				print "\t\t\t$attrs[$desc]\n";
				break;
			case "SKILLS":
				print "\t\t\t<p>\n\t\t\t\t<b>$skills</b>\n";
				print "\t\t\t\t<ul>\n";
				break;
			case "ABILITY":
				$text = "TEXT" . $lang;
				print "\t\t\t\t\t<li>$attrs[$text]</li>\n";
				break;
		}
	}

	function endElement($parser, $name) {
		global $lang, $bgcolor, $memAlign;
		global $bgcolor1, $bgcolor2, $bgcolor3;

		switch ($name) {
			case "MEMBER":
				print "\t\t</td>\n";

				if ($memAlign == 1)
					printMember ();

				if ($memAlign == 0)
					$memAlign = 1;
				else
					$memAlign = 0;

				if ($bgcolor == $bgcolor1)
					$bgcolor = $bgcolor2;
				else
					$bgcolor = $bgcolor1;

				print "\t</tr>\n";
				break;
			case "MEMBERS":
				print "</table>\n";
				break;
			case "SKILLS":
				print "\t\t\t\t</ul>\n\t\t\t</p>\n";
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
