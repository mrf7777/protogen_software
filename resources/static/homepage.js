function createAppElement(app_id) {
    return document.createTextNode(app_id)
}

let app_list = document.getElementById("app-list")
getAppIds((app_ids) => {
    app_ids.forEach(app_id => {
        let app_list_item = document.createElement("li")
        app_list_item.appendChild(createAppElement(app_id))
        app_list.appendChild(app_list_item)
    });
})