<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">

<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
        <title>SELink: Gateway Configuration</title>
        
        <!-- EditableGrid -->
        <script src="{{ static }}/editablegrid/editablegrid.js"></script>
        <script src="{{ static }}/editablegrid/editablegrid_renderers.js" ></script>
        <script src="{{ static }}/editablegrid/editablegrid_editors.js" ></script>
        <script src="{{ static }}/editablegrid/editablegrid_validators.js" ></script>
        <script src="{{ static }}/editablegrid/editablegrid_utils.js" ></script>
        <script src="{{ static }}/editablegrid/ipcellvalidator.js" ></script>
        <link rel="stylesheet" href="{{ static }}/editablegrid/editablegrid.css" type="text/css" media="screen">
        
        <!-- jQuery / jQuery UI -->
        <script src="{{ static }}/jquery/jquery.js" ></script>
        <link rel="stylesheet" href="{{ static }}/jquery/jquery-ui.min.css" type="text/css" media="screen">
        <link rel="stylesheet" href="{{ static }}/jquery/jquery-ui.theme.min.css" type="text/css" media="screen">
        <script src="{{ static }}/jquery/jquery-ui.min.js" ></script>
        
        <!-- Font Awesome -->
        <link rel="stylesheet" href="{{ static }}/font-awesome/css/font-awesome.css" type="text/css" media="screen">

        <!-- JSON -->
        <script src="{{ static }}/json2.js" ></script>
        
        <style>
            body { font-family:'lucida grande', tahoma, verdana, arial, sans-serif; font-size:11px; }
            h1 { text-align:center; font-size: 1.8em; color:#167ead; }
            a { color: #548dc4; text-decoration: none; }
            a:hover { text-decoration: underline; }
            table.mygrid { border-collapse: collapse; border: 1px solid #CCB; width: 100%; }
            table.mygrid td, table.mygrid th { padding: 5px; border: 1px solid #E0E0E0; }
            table.mygrid td {font-weight:normal;}
            table.mygrid th { background: #E5E5E5; }
            table.mygrid th a, table.mygrid th i, table.mygrid td i { color:#167ead; }
            input.invalid { background: red; color: #FDFDFD; }
            i.action-button{font-size:1.6em; cursor: pointer; cursor: hand;}
            
            th.editablegrid-description, td.editablegrid-description, td.editablegrid-description input
            { text-align: left !important; }
            
            th.editablegrid-listen-port, td.editablegrid-listen-port, td.editablegrid-listen-port input,
            th.editablegrid-redirect-host, td.editablegrid-redirect-host, td.editablegrid-redirect-host input,
            th.editablegrid-redirect-port, td.editablegrid-redirect-port, td.editablegrid-redirect-port input,
            th.editablegrid-key-id, td.editablegrid-key-id, td.editablegrid-key-id input
            { text-align:center !important; }
        </style>
        
        <script>
            function addRow(pos)
            {
                if(pos==-1){
                    pos=1+editableGrid.data.length;
                }
                var maxport=0;
                var maxkey=1;
                var regexnew=/^New #([1-9][0-9]*)$/;
                var maxnew=0;
                
                for(var i=0; i<editableGrid.data.length; i++){
                    var row=editableGrid.data[i].columns;
                    if(1*row[1] > maxport)
                        maxport = 1*row[1];
                    if(1*row[3] > maxport)
                        maxport = 1*row[3];
                    if(1*row[4] > maxkey)
                        maxkey = 1*row[4];
                    var m=regexnew.exec(row[0]);
                    if(m!=null){
                        if(1*m[1]>maxnew){
                            maxnew=1*m[1];
                        }
                    }
                }
                editableGrid.addRow(
                    pos, 
                    {
                        "description":"New #"+(maxnew+1),
                        "listen-port": (maxport+1),
                        "redirect-host":"127.0.0.1",
                        "redirect-port":(maxport+2),
                        "key-id": maxkey
                    },
                    {},
                    true
                );
            }
            
            function getGridData()
            {
                data=[];
                for(var i=0; i<editableGrid.data.length; i++){
                    row = editableGrid.data[i].columns;
                    data.push({
                        "description": row[0], 
                        "listen-port": row[1], 
                        "redirect-host": row[2], 
                        "redirect-port": row[3], 
                        "key-id": row[4]
                    });
                }
                return JSON.stringify({"map": data})
            }
        
            $(function() {
                //$(document).tooltip();
            
                editableGrid = new EditableGrid("ProxyRules"); 
                editableGrid.tableLoaded = function() {
                    this.addCellValidator("redirect-host", new IPCellValidator());
                    this.setCellRenderer("action", new CellRenderer({render: function(cell, value) {
                        var rowId = editableGrid.getRowId(cell.rowIndex);
                        var deleteButton=$("<a>");
                        deleteButton.click(function(){
                            editableGrid.remove(cell.rowIndex);
                        });
                        var img = $("<i>");
                        img.addClass("fa")
                            .addClass("fa-trash")
                            .addClass("action-button")
                            .attr("title","Delete")
                            .css("width","100%")
                            .css("text-align","center");
                        deleteButton.append(img);
                        $(cell).append(deleteButton);
                    }}));
                    
                    var headerRenderer= function(tooltip){
                        return new CellRenderer({render: function(cell, value){
                            var img=$("<i>");
                            img.addClass("fa").addClass("fa-info-circle")
                                .css("font-size","1.4em")
                                .css("padding-right","5px")
                                .css("cursor","default")
                                .attr("title",tooltip);
                            $(cell).prepend(img);
                        }});
                    }
                    
                    this.setHeaderRenderer("description", headerRenderer(
                        "A name to identify this port mapping"
                    ));
                    this.setHeaderRenderer("listen-port", headerRenderer(
                        "The gateway will accept connections on this port"
                    ));
                    this.setHeaderRenderer("redirect-host", headerRenderer(
                        "Your server's host (usually ::1)"
                    ));
                    this.setHeaderRenderer("redirect-port", headerRenderer(
                        "Your server's port"
                    ));
                                        
                    this.renderGrid("tablecontent", "mygrid");
                };
                
                $.ajax({
                    type: "GET",
                    url: "get/{{ token }}",
                    success: function(data){
                        if("err" in data){
                            if(data["err"]!=0){
                                if(!("msg" in data)){ data["msg"]="Unknown Error"; }
                                $("#dialog-error-text").text("Operation failed: "+data["msg"]);
                                $("#dialog-error").dialog("open");
                            }
                        }
                        else{
                            editableGrid.loadJSONFromString(data);
                            editableGrid.tableLoaded();
                        }
                    }
                });
                 
                $("#dialog-success").dialog({
                    autoOpen: false,
                    modal: true,
                    buttons: {
                        Ok: function() {
                            $( this ).dialog( "close" );
                        }
                    }
                });
                
                $("#dialog-error").dialog({
                    autoOpen: false,
                    modal: true,
                    buttons: {
                        Ok: function() {
                            $( this ).dialog( "close" );
                        }
                    }
                });
                
                $("#btn-apply").button().click( function() {
                    $.ajax({
                        type: "POST",
                        url: "set/{{ token }}",
                        data: getGridData(),
                        dataType: "json",
                        success: function(data){
                            if(data["err"] == 0){
                                $("#dialog-success-text").text("Your settings were successfully updated");
                                $("#dialog-success").dialog("open");
                            }
                            else {
                                if(!("msg" in data)){ data["msg"]="Unknown Error"; }
                                $("#dialog-error-text").text("Operation failed: "+data["msg"]);
                                $("#dialog-error").dialog("open");
                            }
                        }
                    });
                });
                
                $("#btn-add").button().click(function(){
                    addRow(-1);
                });
                
                $("#btn-cancel").button().click(function(){
                    history.go(0);
                });
                
                $(".gwbutton").css("width","100px");
            });

        </script>
        
    </head>
    
    <body>
        
        <div style="width:80%; margin:auto; padding:10px;">
            <h1>SELink Gateway Configuration</h1>
            <div id="tablecontent" style="width:100%;"></div>
            <div style="height:10px"></div>
            <div style="text-align:right">
                <div id="btn-apply" class="gwbutton" ><i class="fa fa-check"></i>&nbsp;Apply</div>
                <div id="btn-add" class="gwbutton" ><i class="fa fa-plus"></i>&nbsp;Add</div>
                <div id="btn-cancel" class="gwbutton" ><i class="fa fa-times"></i>&nbsp;Cancel</div>
            </div>
        </div>
        <div id="dialog-success" title="SELink">
            <p>
                <i class="fa fa-check"></i>
                <span id="dialog-success-text">dialog-success-text</span>
            </p>
        </div>
        <div id="dialog-error" title="SELink">
            <p>
                <i class="fa fa-times"></i>
                <span id="dialog-error-text">dialog-error-text</span>
            </p>
        </div>
    </body>
</html>
