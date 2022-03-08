# Use the login API to save an access token (as a cookie):
``` 
curl -X 'POST'
   --cookie-jar ./cookie
  'https://fabman.io/api/v1/user/login'
  -H 'accept: application/json'
  -H 'Content-Type: application/json'
  -d 
  '{
      "emailAddress": "XXXX@XXX.com",
      "password": "XXXXX"
   }'
   
Response:   
{
  "members": [
    {
      "id": 236922,
      "memberNumber": "9",
      "firstName": "Kit",
      "lastName": "McCormick"
      
    }
  ],
  "emailAddress": "kit.mccormick@gmail.com",
  "superuser": null,
  "createdAt": "2022-02-21T23:31:19.038Z",
  "updatedAt": "2022-02-21T23:31:19.038Z",
  "state": "active"
}
 ``` 



# Use the cookie for subsequent API calls.  EG: Get the user's packages:
```
curl -X 'GET'
   --cookie ./cookie
  'https://fabman.io/api/v1/members/236922/packages?limit=50'
  -H 'accept: application/json'
  
  Response:
  [
    {
      "id": 52517,
      "package": 4183,
      "fromDate": "2022-03-01"
    },
    {
      "id": 52349,
      "package": 4192,
      "fromDate": "2019-10-13"
    }
  ]
```

The response JSON has been edited for privacy and brevity.