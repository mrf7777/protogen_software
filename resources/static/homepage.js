function launchAppButtonId(app_id) {
    return `launch-app-button-${app_id}`
}

function updateLaunchButton(app_id) {
    const launch_button_id = launchAppButtonId(app_id)
    const launch_button = document.getElementById(launch_button_id)
    getAppIsActive(app_id, (app_is_active) => {
        if(app_is_active) {
            launch_button.innerHTML = ""
            launch_button.appendChild(document.createTextNode("Active"))
            launch_button.classList.remove("btn-primary")
            launch_button.classList.add("btn-success")
        } else {
            launch_button.innerHTML = ""
            launch_button.appendChild(document.createTextNode("Launch"))
            launch_button.classList.remove("btn-success")
            launch_button.classList.add("btn-primary")
        }
    })
}

function createAppCard(app_id, app_name, app_description, app_thumbnail, app_homepage) {
    let card = document.createElement("div")
    card.className = "card flex-row align-items-center"

    let img = document.createElement("img")
    img.className = "object-fit-contain rounded "
    img.style = "max-width: 25vw; height: auto; max-height: 256px;"
    img.src = app_thumbnail
    img.alt = app_name

    let card_body = document.createElement("div")
    card_body.className = "card-body"

    let card_title = document.createElement("h2")
    card_title.className = "card-title"
    card_title.appendChild(document.createTextNode(app_name))

    let card_description = document.createElement("p")
    card_description.className = "card-text"
    card_description.appendChild(document.createTextNode(app_description))

    let card_launch = document.createElement("button")
    card_launch.id = launchAppButtonId(app_id)
    card_launch.className = "btn"
    card_launch.addEventListener("click", () => {
        setAppActive(app_id)
        window.open(app_homepage, "_blank").focus()
    })
    setInterval(() => {
        updateLaunchButton(app_id)
    }, 1000)

    card_body.appendChild(card_title)
    card_body.appendChild(card_description)
    card_body.appendChild(card_launch)

    card.appendChild(img)
    card.appendChild(card_body)

    return card
}

function createAppElement(app_id, callback) {
    let card_element = null
    getAppName(app_id, app_name => {
        getAppDescription(app_id, app_description => {
            getAppHomepage(app_id, app_homepage => {
                getAppThumbnail(app_id, app_thumbnail => {
                    card_element = createAppCard(app_id, app_name, app_description, app_thumbnail, app_homepage)
                    callback(card_element)
                })
            })
        })
    })
}

let app_list = document.getElementById("app-list")
getAppIds((app_ids) => {
    app_ids.forEach(app_id => {
        createAppElement(app_id, (app_element => {
            let app_list_item = document.createElement("li")
            app_list_item.appendChild(app_element)
            app_list.appendChild(app_list_item)

            updateLaunchButton(app_id)
        }))
    });
})