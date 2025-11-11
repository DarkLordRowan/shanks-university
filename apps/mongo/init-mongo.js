db = db.getSiblingDB('shanks');

db.getSiblingDB('admin').createUser({
  user: 'app_user',
  pwd:  process.env.APP_USER_PASSWORD || 'apppass',
  roles: [
    { role: 'readWrite', db: 'shanks' }
  ]
});

db.createCollection('jobs');
db.createCollection('documents');

db.jobs.createIndex({ uuid: 1 }, { unique: true });
db.jobs.createIndex({ status: 1, created_at: -1 });

db.documents.createIndex({ uuid: 1, kind: 1, format: 1 });
db.documents.createIndex({ created_at: -1 });
