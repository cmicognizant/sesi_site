<?php
module_load_include('inc', 'query_interface', 'query_interface.handling_terms');

/**
 * Changes the form elements of the mica query screen
 * Implements hook form_FORM_ID_alter
 * 
 * @param type $form
 * @param type $form_state
 * @param type $form_id 
 */
function query_interface_form_mica_query_edit_form_alter(&$form, $form_state, $form_id) {
    if (ORIGINAL_FORM)
        return $form;

    global $user;
    
    // If no query has been created, create a new one and 
    // store it in cache
    $dataset = $form_state['build_info']['args'][0];
    $query = $form_state['build_info']['args'][1];
    if (!isset($query)) {
        $query = _query_interface_new_query($form_state);
        $form_state['build_info']['args'][1] = $query;
    }

    // Make sure to post the form to the edit-action
    $form["#action"] = url("node/" . $dataset->nid . "/queries/" . $query->id . "/edit");

    // Make sure the name field is filled
    // This is needed because the name field is required
    if (!$form["general"]["name"]["#default_value"])
        $form["general"]["name"]["#default_value"] = "Query " . time() . "." . rand(0, 10000);
    
    
    // Hide the description and publish checkbox from the screen
    $form["general"]["#title"] = t('General');    
    $form["general"]["description"]["#type"] = "hidden";    
    
    // Determine whether the user can put the query to published
    if (in_array('administrator', $user->roles)) {
        $form["general"]["publish"]["#title"] = "Public"; 
    } else {
        $form["general"]["publish"]["#type"] = "hidden";    
    }

    // Add a hidden submit button to be used for persisting query details
    // when adding or editing a term.
    // This button is added as first button in the form, as that is the
    // default button when the form is submitted programmatically.
    $form['general']['tmp-store'] = array(
        '#type' => 'submit',
        '#value' => t('Store temporarily'),
        '#attributes' => array( 'style' => 'display: none;' )
    );    
    
    // By default, show a list of all variables, with the possibility to add
    // and remove constraints for the variable.
    // First remove the add-term box, if it exists
    $unselected_variables = array();
    if (array_key_exists("add-term", $form)) {
        // Find a set of variables not selected before
        $unselected_variables = $form["add-term"]["variables"]["#options"];

        // Hide the variables select option from the screen
        // We also add an id attribute, so we can fill the value
        $form["add-term"]["#description"] = "";
        $form["add-term"]["variables"]["#type"] = "hidden";
        $form["add-term"]["variables"]["#attributes"] = array("id" => "edit-variables");
        unset($form["add-term"]["add-variable"]);

        // Make sure the variables input field has all options that are available
        // Otherwise, the validation of the form may break
        if (!array_key_exists("#options", $form["add-term"]["variables"])) {
            $form["add-term"]["variables"]["#options"] = array();
        }
        if (array_key_exists("list", $form)) {
            foreach ($form["list"] as $key => $element) {
                if (is_numeric($key)) {
                    $form["add-term"]["variables"]["#options"][$element["vid"]["#value"]] = $element["title"]["#markup"];
                }
            }
        }

        // Set default value, as it is needed to validate the form
        $validIds = array_keys($form["add-term"]["variables"]["#options"]);
        $form["add-term"]["variables"]["#value"] = $validIds[0];
    }

    // Hide all weight elements
    foreach ($form["list"] as $key => $element) {
        if (is_numeric($key)) {
            $form["list"][$key]["weight"]["#type"] = "hidden";
        }
    }

    // Add these notselected variables to the list
    $form["not-selected"] = array();
    foreach ($unselected_variables as $vid => $variableTitle) {
        $form["not-selected"][] = array(
            'title' => array(
                '#markup' => $variableTitle,
            ),
            'expression' => array(
                '#type' => 'hidden',
                '#value' => $variableTitle,
            ),
            'weight' => array(
                '#type' => 'hidden',
                '#delta' => 1,
                '#default_value' => 0,
            ),
            'id' => array(
                '#type' => 'hidden',
                '#value' => 0,
            ),
            'vid' => array(
                '#type' => 'hidden',
                '#value' => $vid,
            ),
        );
    }

    // Types of rows in the form:
    //   The key 'list' defines the list of variables for which a constraint has been
    //       added. Every such row should have 2 buttons, 'edit' and 'delete'. Because
    //       of the way mica has been programmed, the remove button must be inside
    //       the 'remove-term' element. That is what the value denotes.
    //   The key 'not-selected' defines the list of variables that can still be selected.
    //       the buttons for these variables (only add button) will be added to 
    //       an element with the name 'buttons-not-selected
    $rowTypes = array("list" => "remove-term", "not-selected" => "buttons-not-selected");
    foreach ($rowTypes as $type => $buttonKey) {
        $form[$buttonKey] = array();
        if (array_key_exists($type, $form)) {
            // Make sure the buttons for all variables exist in the form
            // That is needed for drupal to handle them later on.
            // They are put in the right position in the sesi_mica_query_edit_form method
            foreach ($form[$type] as $key => $variable) {
                if (!is_numeric($key)) {
                    continue;
                }

                $vid = $variable["vid"]["#value"];
                $termId = $variable["id"]["#value"];
                $alreadySelected = ( $type == "list" );

                if ($alreadySelected) {
                    $buttons = array(
                        "edit" => array(
                            "#markup" => ctools_modal_text_button(
                                    t('Edit'), 'sesi/nojs/node/' . $dataset->nid . '/queries/' . $query->id . '/term/' . $termId . '/edit', t('Edit constraint'), 'btn ctools-modal-node-style highlight'
                            ),
                        ),
                        "delete" => array(
                            "#type" => "submit",
                            "#name" => "delete-term-" . str_replace(" ", "", $termId),
                            "#value" => "Delete",
                            "#parents" => array("remove-term"),
                            "#vid" => $vid,
                            "#termid" => $termId,
                            "#submit" => array("query_interface_delete_query_term"),
                        )
                    );
                } else {
                    $buttons = array(
                        "add" => array(
                            "#markup" => ctools_modal_text_button(
                                    t('Add'), 'sesi/nojs/node/' . $dataset->nid . '/queries/' . $query->id . '/term/' . $vid . '/add', t('Add constraint'), 'btn btn-success ctools-modal-node-style highlight'
                            ),
                        )
                    );
                }

                $form[$buttonKey][$key] = $buttons;
            }
        }
    }

    $form["fulltext-search"] = array(
        "#type" => "textfield",
        "#attributes" => array("id" => "edit-search-api-views-fulltext"),
        '#title' => t('Fulltext search')
    );

    // Make sure the taxonomy tree is created from the right URL
    $form["#taxonomy-url"] = url("sesi/node/" . $dataset->nid . "/queries/" . $query->id . "/taxonomy");       
    
    return $form;
}

/**
 * Registers the theme method for changing the look and feel of the query edit form
 * 
 * Please notice that changes to this method will only take effect after 
 * clearing the cache in drupal
 * 
 * Implements hook_theme_registry_alter
 * @param string $theme_registry 
 */
function query_interface_theme_registry_alter(&$theme_registry) {
    if (!empty($theme_registry['mica_query_edit_form']['function'])) {
        $theme_registry['mica_query_edit_form']['function'] = 'sesi_mica_query_edit_form';
    }
}

/**
 * Changes the look and feel of the query edit screen. Is registered as theme function 
 * in the query_interface_theme_registry_alter method
 * @param type $variables
 * @return type 
 * @see query_interface_theme_registry_alter()
 */
function sesi_mica_query_edit_form($variables) {
    if (ORIGINAL_FORM)
        return theme_mica_query_edit_form($variables);

    ctools_add_css('mica_datasets', 'mica_datasets');

    mica_core_modal_prepare(array(
        array(
            'name' => 'edit-style',
            'width' => 600,
            'height' => 500,
        ),
    ));

    $form = $variables['form'];
    $output = "";
    
    // Add taxonomy tree
    drupal_add_js(drupal_get_path('module', 'query_interface') . '/aciTree/js/jquery.aciPlugin.min.js');
    drupal_add_js(drupal_get_path('module', 'query_interface') . '/aciTree/js/jquery.aciTree.dom.js');
    drupal_add_js(drupal_get_path('module', 'query_interface') . '/aciTree/js/jquery.aciTree.core.js');
    drupal_add_js(drupal_get_path('module', 'query_interface') . '/aciTree/js/jquery.aciTree.selectable.js');
    drupal_add_js(drupal_get_path('module', 'query_interface') . '/aciTree/js/jquery.aciTree.checkbox.js');
    drupal_add_css(drupal_get_path('module', 'query_interface') . '/aciTree/css/aciTree.css');
    
    // Add custom interface scripts
    drupal_add_js(drupal_get_path('module', 'query_interface') . '/query_interface.filter-variables.js');
    drupal_add_js(drupal_get_path('module', 'query_interface') . '/query_interface.taxonomy_tree.js');
    drupal_add_css(drupal_get_path('module', 'query_interface') . '/query_interface.css');
    
    $output .= "<div id='query_interface_tree'>";
    $output .= "<a href='#' onClick='Sesi.QueryInterface.TaxonomyTree.selectAll(); return false;'>Select all</a> / ";
    $output .= "<a href='#' onClick='Sesi.QueryInterface.TaxonomyTree.selectAll(false); return false;'>Deselect all</a>";
    $output .= "<div id='taxonomy_tree' class='aciTree' data-url='" . htmlspecialchars( $form[ '#taxonomy-url' ] ). "'></div>";
    $output .= "</div>";
    
    $output .= "<style> #edit-actions{ text-align:right !important; } </style>";
    
    $output .= "<div id='query_screen'>";
    
    // Add fulltext search
    $output .= drupal_render($form["fulltext-search"]);

    // Types of rows in the form:
    //   The key 'list' defines the list of variables for which a constraint has been
    //       added. Every such row should have 2 buttons, 'edit' and 'delete'. Because
    //       of the way mica has been programmed, the remove button must be inside
    //       the 'remove-term' element. That is what the value denotes.
    //   The key 'not-selected' defines the list of variables that can still be selected.
    //       the buttons for these variables (only add button) will be added to 
    //       an element with the name 'buttons-not-selected
    $output .= _query_interface_render_query_terms_table($form["list"], $form["remove-term"]);
    
    $output .= drupal_render_children($form, array('actions'));      
    
    $output .= _query_interface_render_variables_table($form["not-selected"], $form["buttons-not-selected"]);

    $output .= drupal_render_children($form, array('cross'));
    $output .= drupal_render_children($form, array('general'));
    

    // add form hidden items, otherwise submit and validate will be ignored
    $fieldsAlreadyPrinted = array('general', 'cross', 'actions', 'list', 'remove-term', 'not-selected', 'buttons-list', 'buttons-not-selected', 'fulltext-search');
    foreach (element_children($form) as $key) {
        if (!in_array($key, $fieldsAlreadyPrinted)) {
            $output .= drupal_render($form[$key]);
        }
    }
    
    $output .= "</div>"; // Query screen

    return $output;
}

/**
 * Create a new query in cache, without any options yet
 * @global type $user
 * @param type $form_state
 * @return stdClass Newly created query 
 */
function _query_interface_new_query($form_state) {
    $dataset_node = $form_state['build_info']['args'][0];
    global $user;

    // Create a set of values to store the query
    $values = array();
    $values['name'] = "Query " . time() . "." . rand(0, 10000);
    $values['description'] = "";
    $values['dataset_id'] = $dataset_node->nid;
    $values['cross_variable_id'] = null;
    $values['user_id'] = $user->uid;
    $values['module'] = 'mica_query';

    // Store this query in cache
    return MicaDatasetQueryManager::addQuery($values);
}

/**
 * Renders a table with existing queryterms in a query form
 * @param type $termList    A list of selected query terms
 * @param type $buttonList  A list with the buttons to show for each term.
 */
function _query_interface_render_query_terms_table($termList, $buttonList) {
    $renderRowFunction = "_query_interface_render_query_term_row";
    $tableHeader = array(t('Query Term'), t('Expression'), t('Operations'));

    return _query_interface_render_table($termList, $buttonList, $renderRowFunction, $tableHeader, "queryterms");
}

/**
 * Renders a table with variables not yet used in a query term
 * @param type $form 
 */
function _query_interface_render_variables_table($termList, $buttonList) {  
    
    $renderRowFunction = "_query_interface_render_variable_row";
    $tableHeader = array(t('Query Term'), t('Label'), t('Description'), t('Operations')); //PARTHA

    return _query_interface_render_table($termList, $buttonList, $renderRowFunction, $tableHeader, "variables");
}

/**
 * Generic function to show a table on the query screen
 * @param type $termList
 * @param type $buttonList
 * @param type $renderRowsFunction
 * @param type $tableHeader 
 */
function _query_interface_render_table($termList, $buttonList, $renderRowsFunction, $tableHeader, $id = null) {
    $rows = array();    
      
     
    foreach (element_children($termList) as $key) {
        $variable = $termList[$key];       
       
        
        $alreadySelected = true;

        if (array_key_exists($key, $buttonList)) {
            $buttons = drupal_render_children($buttonList[$key]);
        } else {
            $buttons = null;
        }

        $currentTermRows = call_user_func($renderRowsFunction, $variable, $buttons);
       
       
        
        $rows = array_merge($rows, $currentTermRows);
    }

    // Renders the table itself, if any rows are present
    if (count($rows) > 0) {
        return theme('table', array(
                    'header' => $tableHeader,
                    'rows' => $rows,
                    'empty' => t('No data found'),
                    'attributes' => array('id' => $id ? $id : "") // Needed for searching
                ));
    } else {
        return "";
    }
}

/**
 * Render table rows for a query term
 * @param type $variable
 * @param type $buttons
 * @return array    Each element in the array should be a row that can be passed to the theme_table method.
 * @see https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
 */
function _query_interface_render_query_term_row($variable, $buttons) {
    // Loading the query term again. This is not a very performant solution
    // as the query term has been loaded in the mica code as well. However
    // the mica code only provides us with the 'title', where we need
    // a separate variable name and expression.
    // However, the query term doesn't have a separate method to get a 
    // human readable expression. For that reason, we use the original
    // title, and remove the variable name from it.
    // TODO: Determine a better way to do this
    $queryTerm = mica_query_term_load($variable["id"]["#value"]);

    // Split the variable name and expression
    $completeExpression = $variable["title"]["#markup"];
    $variableName = $queryTerm->termImpl()->getFieldName();
    $onlyExpression = strlen($completeExpression) > strlen($variableName) ? substr($completeExpression, strlen($variableName) + 1) : "";

    // Put the variable name and expression in the right places
    $variable["title"]["#markup"] = $variableName;
    $variable["humanreadableexpression"] = array("#markup" => $onlyExpression);

    $row = array();

    // title
    $row[] = drupal_render($variable['title']);

    // weight and hidden fields
    $weightElement = $variable['weight'];
    $weightElement["#type"] = "hidden";
    $column = drupal_render($weightElement);

    // This expression is set by mica itself
    if (array_key_exists('expression', $variable)) {
        $column .= drupal_render($variable['expression']);
    }

    // Add hidden elements to store id and vid for this element
    $column .= drupal_render($variable['id']) . drupal_render($variable['vid']);

    // Also put the expression itself in the column
    if (array_key_exists('humanreadableexpression', $variable)) {
        $column .= drupal_render($variable['humanreadableexpression']);
    }

    $row[] = $column;

    // Add operations
    $row[] = $buttons;

    return array(array("data" => $row));
}

/**
 * Render table rows for a variable
 * @param type $variable
 * @param type $buttons
 * @return array    Each element in the array should be a row that can be passed to the theme_table method.
 * @see https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
 */
function _query_interface_render_variable_row($variable, $buttons) {
    
    //print_r($variable);
    //echo "<br/>========================== <br/>";
    
    // Loading the variable again. This is not a very performant solution
    // as the variable has been loaded in the mica code as well. However
    // the mica code only provides us with the 'title', where we need
    // to show the description and taxonomy as well.
    // TODO: Determine a better way to do this
    $variableObject = node_load($variable["vid"]["#value"]);
    $taxonomies = array();

    if ($variableObject) {
        $wrapper = entity_metadata_wrapper("node", $variableObject);

        // Check if a description is present
        if ($wrapper->body->value() && $wrapper->body->value) {
            $variable["description"] = $wrapper->body->value->value();
        }
        //Set field label
        $variable["field_label"] = $wrapper->field_label->value(); //PARTHA
        
        // Retrieve taxonomies and show them in the page
        $taxonomies = _query_interface_get_taxonomies($wrapper);
    }

    // Create the rows itself
    $rows = array();

    // Create the first row with title, description and buttons
    $rowData = array();


    // title
    $rowData[] = drupal_render($variable['title']);     
    
    $rowData[] = $variable["field_label"];  //PARTHA 
    
    // weight and hidden fields
    $weightElement = $variable['weight'];
    $weightElement["#type"] = "hidden";
    $column = drupal_render($weightElement);    
    
    
    // Add the description to the table
    // The description may contain markup, and is for that reason not escaped
    if (array_key_exists("description", $variable))
        $column .= $variable["description"];   
    
    
    $rowData[] = $column; 
    
   
     
    // Add operations
    $rowData[] = $buttons;

    $rows[] = array("data" => $rowData, "class" => array("variable_" . $variable['vid']["#value"]));
    
   
    
   
    //$rows[] = array("data" => $rowData, "class" => array("variable_" . $variable['vid']["#value"]));

    // Create the second row with taxonomies. For each taxonomy, we also
    // add a hidden element with the taxonomy id
    // This time, the cell needs HTML properties being set,
    // so the structure is somewhat more complex
    $taxonomyDescription = implode(", ", array_map(
                    function($taxonomy) {
                        $output = '<input type="hidden" class="hidden-taxonomy-id" value="' . $taxonomy->tid . '" />';
                        return $output . htmlspecialchars($taxonomy->name);
                    }, $taxonomies));

    // Add a row with one cell (colspan=3)
    $rows[] = array(
        "data" => array(
            array(
                "data" => $taxonomyDescription,
                "colspan" => 3
            ),
        ),
        "class" => array("variable_" . $variable['vid']["#value"])
    );

    return $rows;
}