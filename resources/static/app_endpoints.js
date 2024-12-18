const origin = window.location.origin

function getAppIds(callback) {
    fetch(`${origin}/protogen/apps`)
        .then(response => response.text())
        .then(text => {
            const app_ids = text.split(/\r?\n/).filter(app_id => app_id !== "")
            app_ids.sort()
            callback(app_ids)
        })
}

function getAppName(app_id, callback) {
    fetch(`${origin}/protogen/apps/${app_id}/name`)
        .then(response => response.text())
        .then(text => {
            callback(text)
        })
}

function getAppDescription(app_id, callback) {
    fetch(`${origin}/protogen/apps/${app_id}/description`)
        .then(response => response.text())
        .then(text => {
            callback(text)
        })
}

function getAppThumbnail(app_id, callback) {
    fetch(`${origin}/protogen/apps/${app_id}/thumbnail`)
        .then(response => response.text())
        .then(url => {
            callback(url)
        })
}

function getAppIsActive(app_id, callback) {
    fetch(`${origin}/protogen/apps/${app_id}/active`)
        .then(response => response.text())
        .then(text => {
            callback(text === "true")
        })
}

function setAppActive(app_id) {
    fetch(`${origin}/protogen/apps/${app_id}/active`, {method: "put"})
}

function getActiveAppId(callback) {
    fetch(`${origin}/protogen/apps/${app_id}/description`)
        .then(response => response.text())
        .then(text => {
            callback(text)
        })
}

function getAppHomepage(app_id, callback) {
    fetch(`${origin}/protogen/apps/${app_id}/homepage`)
        .then(response => response.text())
        .then(text => {
            callback(text)
        })
}

function getAppHomepage(app_id, callback) {
    fetch(`${origin}/protogen/apps/${app_id}/homepage`)
        .then(response => response.text())
        .then(text => {
            callback(text)
        })
}