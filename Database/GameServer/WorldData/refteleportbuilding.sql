DROP TABLE IF EXISTS `refteleportbuilding`;
CREATE TABLE  `refteleportbuilding` (
	`ID` int(11) NOT NULL,
	`Zone` int(11) NOT NULL,
	`X` int(11) NOT NULL,
	`Z` int(11) NOT NULL,
	`Y` int(11) NOT NULL,
	`TaxFrom` INTEGER NOT NULL,
	`AvailableTo` INTEGER NOT NULL
);
