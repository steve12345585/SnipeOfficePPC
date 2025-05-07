ExampleDataPilotSource
======================

This example shows how you can implement an external DataPilot
component.

Simple usage scenario:
======================
1. build and deploay the component
2. open a new spreadsheet document
3. select for example cell B:2
4. start the DataPilot dialog
   Data->DataPilot->Start...
5. select "External source/interface -> Ok
6. select the example DataPilot 
   Use the the drop down list and select the service
   "ExampleDataPilotSource" -> Ok
7. Place some number fields in the field areas
   For example: 
   - select the "ones" field and drop it in the "Row Fields" area
   - repeat this step with the "tens" and "hundreds" field
   - select the "thousands" field and drop it in the "Column Fields" 
     area.
   -> press Ok
8. you see a matrix with rows and colums for the selected fields
   and a value area with a sum up of all fields valid for this 
   coordinate. For example cell F:21=1221 (ones=B:21=1, tens=c:21=2
   hundreds=D:21=2 and thousands=F:3=1)

     
