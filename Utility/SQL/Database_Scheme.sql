CREATE TABLE users ( id serial PRIMARY KEY, email varchar(32) UNIQUE NOT NULL, login varchar(128) UNIQUE NOT NULL, password_hash varchar(128) UNIQUE NOT NULL );

CREATE TABLE user_personal_data ( user_id int NOT NULL, first_name varchar(32), second_name varchar(32), birthday date, sex boolean );

CREATE TABLE user_tokens ( user_id int UNIQUE, token varchar(64) NOT NULL, refresh_token varchar(64) NOT NULL, token_receipt_time timestamp NOT NULL );

CREATE TABLE user_friends_data ( user_id int NOT NULL, friend_id int UNIQUE NOT NULL );

CREATE TABLE user_channels ( user_id int NOT NULL, channel_id int NOT NULL, UNIQUE (user_id, channel_id) );

CREATE TABLE msgs ( msg_id serial PRIMARY KEY, sender_id int NOT NULL, send_time timestamp, msg_type CHAR(1) NOT NULL DEFAULT 'T' CHECK (msg_type IN ('T', 'A', 'V', 'I', 'F')));

CREATE TABLE msg_reactions ( msg_id int NOT NULL, likes int[] DEFAULT array[]::int[], dislikes int[] DEFAULT array[]::int[], fires int[] DEFAULT array[]::int[], cats int[] DEFAULT array[]::int[], smiles int[] DEFAULT array[]::int[]);

CREATE TABLE channel_msgs ( channel_id int NOT NULL, msg_id int NOT NULL );

CREATE TABLE channels ( id serial PRIMARY KEY, channel_name varchar(64) UNIQUE, creator_id int NOT NULL, user_limit int NOT NULL );

CREATE TABLE channel_links ( channel_id int NOT NULL, link_id int NOT NULL, save_date timestamp NOT NULL, sender_id int NOT NULL );

CREATE TABLE links ( link_id serial PRIMARY KEY, link varchar(64) NOT NULL );

CREATE TABLE replies ( sender_id int NOT NULL, msg_id_owner int NOT NULL, msg_id_ref int NOT NULL, msg text NOT NULL );

CREATE TABLE channel_replies ( channel_id int NOT NULL, msg_id_owner int NOT NULL );

CREATE TABLE text_msgs (
    msg_id int PRIMARY KEY REFERENCES msgs(msg_id) ON DELETE CASCADE,
    msg TEXT NOT NULL
);

CREATE TABLE voice_msgs (
    msg_id int PRIMARY KEY REFERENCES msgs(msg_id) ON DELETE CASCADE,
    voice_name VARCHAR(128) NOT NULL,
    duration INT NOT NULL CHECK (duration > 0)
);

CREATE TABLE video_msgs (
    msg_id int PRIMARY KEY REFERENCES msgs(msg_id) ON DELETE CASCADE,
    video_name VARCHAR(128) NOT NULL,
    duration INT NOT NULL CHECK (duration > 0)
);

CREATE TABLE image_msgs (
    msg_id int PRIMARY KEY REFERENCES msgs(msg_id) ON DELETE CASCADE,
    image_name VARCHAR(128) NOT NULL
);

CREATE TABLE file_msgs (
    msg_id int PRIMARY KEY REFERENCES msgs(msg_id) ON DELETE CASCADE,
    file_name VARCHAR(128) NOT NULL
);

ALTER TABLE user_personal_data ADD FOREIGN KEY (user_id) REFERENCES users (id) ON DELETE CASCADE;

ALTER TABLE user_tokens ADD FOREIGN KEY (user_id) REFERENCES users (id) ON DELETE SET NULL;

ALTER TABLE user_friends_data ADD FOREIGN KEY (user_id) REFERENCES users (id) ON DELETE SET NULL;

ALTER TABLE user_friends_data ADD FOREIGN KEY (friend_id) REFERENCES users (id) ON DELETE SET NULL;

ALTER TABLE user_channels ADD FOREIGN KEY (user_id) REFERENCES users (id) ON DELETE SET NULL;

ALTER TABLE user_channels ADD FOREIGN KEY (channel_id) REFERENCES channels (id) ON DELETE SET NULL;

ALTER TABLE msg_reactions ADD FOREIGN KEY (msg_id) REFERENCES msgs (msg_id) ON DELETE CASCADE;

ALTER TABLE channel_msgs ADD FOREIGN KEY (channel_id) REFERENCES channels (id) ON DELETE CASCADE;

ALTER TABLE channel_msgs ADD FOREIGN KEY (msg_id) REFERENCES msgs (msg_id) ON DELETE CASCADE;

ALTER TABLE channels ADD FOREIGN KEY (creator_id) REFERENCES users (id) ON DELETE CASCADE;

ALTER TABLE channel_links ADD FOREIGN KEY (channel_id) REFERENCES channels (id) ON DELETE SET NULL;

ALTER TABLE channel_links ADD FOREIGN KEY (link_id) REFERENCES links (link_id) ON DELETE CASCADE;

ALTER TABLE channel_links ADD FOREIGN KEY (sender_id) REFERENCES users (id) ON DELETE SET NULL;

ALTER TABLE channel_replies ADD FOREIGN KEY (channel_id) REFERENCES channels (id) ON DELETE CASCADE;

ALTER TABLE channel_replies ADD FOREIGN KEY (msg_id_owner) REFERENCES msgs (msg_id) ON DELETE CASCADE;

ALTER TABLE replies ADD FOREIGN KEY (msg_id_owner) REFERENCES msgs (msg_id) ON DELETE CASCADE;

ALTER TABLE replies ADD FOREIGN KEY (msg_id_ref) REFERENCES msgs (msg_id) ON DELETE CASCADE;
