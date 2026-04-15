# SEO Implementation Overview

Status as of April 2026. Covers tickets SEO-01 through SEO-05.

---

## Completed

### SEO-01 — generateMetadata for content pages

**File:** `app/[lang]/[...slug]/page.tsx`

Server-side `generateMetadata` using the same `fetchPage(url, lang)` call as the page component (Next.js deduplicates automatically). Reads the flat `metaData` object returned by the gateway (which runs `filterPageByLanguage` to resolve `metaData.i18n[lang]` into a flat shape).

Outputs:
- `<title>`
- `<meta name="description">`
- `<link rel="canonical">` (when `canonicalUrl` is present)
- `og:type` = `"article"`
- `og:title`, `og:description`
- `og:url` (from `metaData.url`)

Fallbacks: title → `"Shop"`, description → `""`.

### SEO-02 — generateMetadata for product detail pages

**File:** `app/[lang]/(system)/product/detail/[id]/page.tsx`

Same pattern as SEO-01 but for the PDP. Reuses `fetchProductPage(id, lang)`.

Additional features:
- Description capped at 155 characters (Google snippet limit)
- `og:image` extracted from `schemaOrg.image` (handles both string and array formats)
- Title fallback chain: `metaData.title` → `pageName` → `"Product"`

### SEO-03 — sitemap.xml for content pages

**File:** `app/sitemap.ts`

Next.js auto-serves this as `/sitemap.xml`. Fetches online content pages from the gateway with server-side filtering (`?type=content&online=true`).

For each page, iterates all supported languages (`en`, `nl`, `fr`, `de`) and builds URLs from `metaData.i18n[lang].canonicalUrl` or `.url`, with a `pageName` fallback.

- Homepage priority: `1.0`, others: `0.8`
- `changeFrequency`: `"weekly"`
- Revalidates every hour (`next: { revalidate: 3600 }`)

Session token resolution handles the fact that `/sitemap.xml` bypasses middleware (path contains a dot): tries middleware header → `shop_session_token` cookie → resolves from hostname.

#### Multi-tenant sitemap — how it works

How does each shop get its own sitemap?

The answer is that tenant isolation happens automatically via the hostname and session token. Each shop has its own domain, and the sitemap resolves the shop context from the incoming request:

```
shop-a.com/sitemap.xml
  → hostname = "shop-a.com"
  → resolves session token scoped to shop-a (via GET /shops/resolvedomain)
  → gateway returns only shop-a's online content pages
  → URLs generated as https://shop-a.com/nl/..., https://shop-a.com/fr/...

shop-b.com/sitemap.xml
  → hostname = "shop-b.com"
  → resolves session token scoped to shop-b
  → gateway returns only shop-b's online content pages
  → URLs generated as https://shop-b.com/nl/..., https://shop-b.com/fr/...
```

The base URL for sitemap entries is derived from the request's `Host` header (not from an env var), so each domain gets correct absolute URLs. `NEXT_PUBLIC_BASE_URL` is only used as a last-resort fallback for local dev.

This mirrors the old ColdFusion app's `sitemap-{shopId}.xml` pattern — same tenant isolation, just dynamic instead of pre-generated static files.

**Backend changes (SEO-03):**
- `ecom2service` — `PageRepository.listPages` and `PageHandlers.getPages` now support `?online=true` filter at the MongoDB query level
- `ecom2gatewayservice` — `CmsHandlers.listPagesHandler` passes `online` query param through; `CMS_LIST_FIELDS` includes `metaData`
- `ecom2-app` — `fetchPages` accepts `online?: boolean` filter

### SEO-04 — GA4 script injection per shop (multi-tenant)

**Files changed across all three projects:**

| Layer | File | Change |
|---|---|---|
| ecom2service | `themes/interfaces/api/Theme.ts` | Added `IThemeAnalytics` with `gaId?: string` to `ITheme` |
| ecom2service | `themes/classes/models/Theme.ts` | Added `ThemeAnalytics` class |
| ecom2service | `themes/classes/mapper/ThemeMapper.ts` | Maps `analytics.gaId` from MongoDB |
| ecom2gatewayservice | `theme/rest-client.ts` | Added `analytics` to `IThemeResponse` |
| ecom2-app | `lib/gateway/themeClient.ts` | Added `analytics` to `ThemeResponse` |
| ecom2-app | `lib/theme/getTheme.ts` | Added `ThemeAnalytics` interface, passes through |
| ecom2-app | `app/[lang]/layout.tsx` | Injects gtag.js + config script when `gaId` present |

GA4 measurement ID is stored in MongoDB per shop: `{ analytics: { gaId: "G-XXXXXXXXXX" } }` on the theme document. Scripts use `strategy="afterInteractive"` to avoid blocking page render. Shops without `gaId` get no script.

### SEO-05 — GA4 pageview tracking on client-side navigation

**File:** `components/analytics/GoogleAnalyticsPageview.tsx`

Client component that listens to `pathname` and `searchParams` changes. On each soft navigation fires `gtag('config', gaId, { page_path })` which triggers a pageview and updates the GA4 session context.

- Guarded with `if (!window.gtag) return`
- Wrapped in `<Suspense fallback={null}>` in layout (Next.js requirement for `useSearchParams`)
- Also declares `window.dataLayer` on the global type for future e-commerce event tracking

---

## Key architectural note — metaData shape

The gateway's `filterPageByLanguage` (in `ecom2gatewayservice`) flattens `metaData.i18n[lang]` into a flat `metaData: { url, title, description, canonicalUrl }` for resolved page responses (`/page` and `/page/product/detail/:id` endpoints).

The raw `metaData.i18n` wrapper only appears on the `/cms/pages` list endpoint (used by the sitemap).

The `generateMetadata` functions in both page files cast accordingly via a `FlatPageMeta` type.

---

## Gaps vs old e-commerce-app (future tickets)

| Feature | Old app | New app | Notes |
|---|---|---|---|
| `og:site_name` | `prc.shopData.title` | ❌ Missing | Needs shop name from gateway |
| `og:image` on content pages | `contentPageData.properties.image` | ❌ Missing | Not in current gateway response |
| `og:image:width/height` | 1000×1000 (products), 1200×630 (content) | ❌ Missing | Needs image dimensions |
| `hreflang` alternates | Built from all languages + subshops | ❌ Missing | Complex multi-shop logic |
| `robots: noindex` | Test domains, agent-only shops, search pages, `noIndex` flag | ❌ Missing | Multiple conditions |
| `rel="prev"` / `rel="next"` | Paginated product listers | ❌ Missing | Lister pages not yet built |
| WebSite SearchAction snippet | `application/ld+json` on every page | ❌ Missing | Schema.org structured data |
| Breadcrumb rich snippets | `application/ld+json` | ❌ Missing | Via `breadcrumbService` |
| Favicon per tenant | Theme static files or CDN via settings | ❌ Missing | Branding, not SEO — add to theme config |
| `theme-color` meta | Per theme `head.cfm` | ❌ Missing | Branding — add to theme config |
| GA4 e-commerce events | `view_item`, `add_to_cart`, `purchase`, etc. | ❌ Missing | Old app's `ga4.cfc` interceptor |
