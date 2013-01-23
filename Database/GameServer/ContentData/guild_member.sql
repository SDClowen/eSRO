DROP TABLE IF EXISTS `guild_member`;
CREATE TABLE  `guild_member` (
	`ID` INTEGER NOT NULL auto_increment,
	`GuildID` INTEGER NOT NULL,
	`Name` varchar(12) NOT NULL,
	`GrantName` varchar(20) NOT NULL,
	`GP` INTEGER NOT NULL,
	`CombatPoints` INTEGER NOT NULL,
	`Level` int(3) NOT NULL,
	`refCharID` int(11) NOT NULL,
	`Master` int(1) NOT NULL,
	`JoinRights` BOOLEAN NOT NULL DEFAULT FALSE,
	`WithdrawRights` BOOLEAN NOT NULL DEFAULT FALSE,
	`UnionRights` BOOLEAN NOT NULL DEFAULT FALSE,
	`StorageRights` BOOLEAN NOT NULL DEFAULT FALSE,
	`NoticeRights` BOOLEAN NOT NULL DEFAULT FALSE,
	`Position` INTEGER(3) NOT NULL DEFAULT 0,
	`Zone` INTEGER NOT NULL DEFAULT 0,
	PRIMARY KEY  (`id`)
);

