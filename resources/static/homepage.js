function createAppElement(app_id) {
    let element = document.createTextNode("")
    getAppName(app_id, app_name => {
        getAppDescription(app_id, app_description => {
            getAppHomepage(app_id, app_homepage => {
                element.textContent = `${app_name} ${app_description} ${app_homepage}`
            })
        })
    })
    return element
}

let app_list = document.getElementById("app-list")
getAppIds((app_ids) => {
    app_ids.forEach(app_id => {
        let app_list_item = document.createElement("li")
        app_list_item.appendChild(createAppElement(app_id))
        app_list.appendChild(app_list_item)
    });
})