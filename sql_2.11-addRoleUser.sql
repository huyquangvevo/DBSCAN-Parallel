INSERT INTO roles(name) VALUES ('public content'),('report content'),('slide1'),('manager');

SET @adminId = (SELECT id FROM users WHERE username='admin');
SET @publicRoleId = (SELECT id FROM roles WHERE name='public content');
UPDATE user_roles SET role_id = @publicRoleId WHERE user_id=@adminId;